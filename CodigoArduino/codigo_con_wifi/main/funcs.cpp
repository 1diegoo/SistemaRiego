#include <DHT.h>

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <ESP8266.h>

//#define DEBUG_MODE

#include "dbg_utils.h"
#include "states.h"
#include "funcs.h"

#define GET_DECIMAL_POS(number) ((int)((number - (int)number) * 10))

namespace wifiparams{
  static constexpr char ssid[] = "RedComun";
  static constexpr char pswd[] = "admin123";
  static constexpr char server[] = "10.0.1.2";
  static constexpr int port = 80;
  static const char requestFormat[] PROGMEM = "GET /srv/api/Updt"
                                               "?Id=%d&Nm=%S&Sh=%d.%1d&Ah=%d.%1d&Tm=%d.%1d&Lm=%d.%1d "
                                               "HTTP/1.1\r\n"
                                               "Host: 10.0.1.2\r\n"
                                               "Connection: close\r\n\r\n";
                                               
  static const char accName[] PROGMEM = "Acc1";
};

void connect(ESP8266& wificon) noexcept {   
  if (wificon.joinAP(wifiparams::ssid, wifiparams::pswd)) {
    RUN_DBG_ONLY(Serial.println(F("Wifi Connected")));
    RUN_DBG_ONLY(Serial.println(F("IP address is: ")));
    RUN_DBG_ONLY(Serial.println(wificon.getLocalIP().c_str()));
    digitalWrite(pins::actionStatus, HIGH);
    
    if (wificon.disableMUX()) {
      RUN_DBG_ONLY(Serial.println(F("single on")));
      delay(1000);
      digitalWrite(pins::actionStatus, LOW);
      switchState(states::reading);
    }
    else {
      RUN_DBG_ONLY(Serial.println(F("single setup failed, retrying")));
      delay(1000);
    }
  }
  else {
    RUN_DBG_ONLY(Serial.println(F("Error trying to connect to a network, trying again...")));
    RUN_DBG_ONLY(Serial.println(F("Make sure the router is called \"RedComun\" and has \"admin123\" as its password")));
    delay(1000);
  }
}

void readData(DHT& aif) {
  readings.humedadAire = aif.readHumidity();
  readings.temperatura = aif.readTemperature();
  if (isnan(readings.humedadSuelo) || isnan(readings.temperatura)) {
    RUN_DBG_ONLY(Serial.println(F("Invalid values, retrying")));
    digitalWrite(pins::actionStatus, HIGH);
    delay(200);
    digitalWrite(pins::actionStatus, LOW);
    delay(200);
    return;
  }

  readings.humedadSuelo = ((analogRead(pins::fc28)/1023.0) * 100);
  readings.luminosidad = ((1023 - analogRead(pins::ldr)/1023.0) * 100);
  switchState(states::sending);
}

void sendReadings(ESP8266& wifimod) {
  if(wifimod.createTCP(wifiparams::server, wifiparams::port)) {
    {
      digitalWrite(pins::actionStatus, HIGH);
      char requestbuf[128];
      sprintf_P(requestbuf, 128, wifiparams::requestFormat, 1, wifiparams::accName,
        (int)readings.humedadSuelo, GET_DECIMAL_POS(readings.humedadSuelo),
        (int)readings.humedadAire, GET_DECIMAL_POS(readings.humedadAire),
        (int)readings.luminosidad, GET_DECIMAL_POS(readings.luminosidad),
        (int)readings.temperatura, GET_DECIMAL_POS(readings.temperatura));

      if(!wifimod.send((const uint8_t*)requestbuf, strlen(requestbuf))) {
        RUN_DBG_ONLY(Serial.println(F("Error while sending data")));
        RUN_DBG_ONLY(Serial.println(F("Retrying to configure connection")));
        for (int i = 0; i < 3; i++) {
          digitalWrite(pins::actionStatus, HIGH);
          delay(150);
          digitalWrite(pins::actionStatus, LOW);
          delay(150);
        }
        switchState(states::searching);
        goto close;
      }
    }

    {
      char recvbuf[300] = {0};
      if(wifimod.recv(recvbuf, 300, 10000) <= 0) {
        RUN_DBG_ONLY(Serial.println(F("Error while receiving data")));
        RUN_DBG_ONLY(Serial.println(F("Retrying to configure connection")));
        switchState(states::searching);
        goto close;
      }

      char* httpStatusString = strtok(recvbuf, "\r\n");
      int statusCode = 0;
      if(sscanf_P(httpStatusString, PSTR("HTTP/1.1 %d %*s"), &statusCode) != 1) {
        RUN_DBG_ONLY(Serial.println(F("Invalid HTTP response...")));
        digitalWrite(pins::actionStatus, LOW);
        switchState(states::waiting);
        goto close;
      }

      if (statusCode == 200) {
        char* endOfHeaders = strstr(recvbuf + strlen(httpStatusString) + 3, "\r\n\r\n");
        if (endOfHeaders == nullptr) {
          RUN_DBG_ONLY(Serial.println(F("Invalid HTTP response...")));
          digitalWrite(pins::actionStatus, LOW);
          switchState(states::waiting);
          goto close;
        }

        auto jsonit = recvbuf;
        for(; (*jsonit != '{') && (*jsonit != 0); jsonit++);
        if(!*jsonit) {
          RUN_DBG_ONLY(Serial.println(F("No body has been found in the response, trying again")));
          digitalWrite(pins::actionStatus, LOW);
          switchState(states::waiting);
          goto close;
        }

        char answer[8];
        sscanf_P(jsonit, PSTR("{\"success\":%*[ ]%s}"), answer);
        int orden = strcmp_P(answer, PSTR("true"));

        if(orden == 0)
          switchState(states::active);
        else {
          digitalWrite(pins::actionStatus, LOW);
          switchState(states::waiting);
        }
          
        goto close;
      }
      else {
        RUN_DBG_ONLY(Serial.println(F("There was an error while processing the request, trying again")));
        digitalWrite(pins::actionStatus, LOW);
        switchState(states::waiting);
      }
    }
  }
  else {
    RUN_DBG_ONLY(Serial.println(F("Couldn't establish a connection with the server")));
    RUN_DBG_ONLY(Serial.println(F("Retrying to configure connection")));
    for (int i = 0; i < 3; i++) {
      digitalWrite(pins::actionStatus, HIGH);
      delay(150);
      digitalWrite(pins::actionStatus, LOW);
      delay(150);
    }
    switchState(states::searching);
    goto end;
  }
  close:
  wifimod.releaseTCP();
  end:
  ;
}

void setOnSleepMode() {
  RUN_DBG_ONLY(Serial.println(F("We haven't received the order to water, we are sleeping for 10 seconds before trying again")));
  delay(10000);
  switchState(states::reading);
}

void activateBomb() {
  RUN_DBG_ONLY(Serial.println(F("Activating the water bomb")));
  digitalWrite(pins::pump, HIGH);
  auto time = millis();
  while((millis() - time) <= 10000);
  digitalWrite(pins::pump, LOW);

  RUN_DBG_ONLY(Serial.println(F("Done watering, reading data")));
  digitalWrite(pins::actionStatus, LOW);
  switchState(states::reading);
}