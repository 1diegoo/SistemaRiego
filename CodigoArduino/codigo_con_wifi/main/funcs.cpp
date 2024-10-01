#include <DHT.h>

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <ESP8266.h>

#define DEBUG_ONLY

#include "dbg_utils.h"
#include "states.h"
#include "funcs.h"

namespace wifiparams{
  static constexpr char ssid[] = "RedComun";
  static constexpr char pswd[] = "admin123";
  static constexpr char server[] = "10.0.1.2";
  static constexpr int port = 5255;
  static const char requestFormat[] PROGMEM = "GET /api/Updt"
                                               "?Id=%d&Nm=%S&Sh=%.1f&Ah=%.1f&Tm=%.1f&Lm=%.1f "
                                               "HTTP/1.1\r\n"
                                               "Host: 10.0.1.2\r\n"
                                               "Connection: close\r\n ";
                                               
  static const char accName[] PROGMEM = "Acc1";
};

void connect(ESP8266& wificon) noexcept {   
  if (wificon.joinAP(wifiparams::ssid, wifiparams::pswd)) {
    RUN_DBG_ONLY(Serial.println(F("Wifi Connected")));
    RUN_DBG_ONLY(Serial.println(F("IP address is: ")));
    RUN_DBG_ONLY(Serial.println(wificonn.getLocalIP().c_str()));
    
    if (wificon.disableMUX()) {
      RUN_DBG_ONLY(Serial.println(F("single on")));
      current_state = states::reading;
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
  lecturas.humedadAire = aif.readHumidity();
  lecturas.temperatura = aif.readTemperature();
  if (isnan(lecturas.humedadSuelo) || isnan(lecturas.temperatura)) {
    RUN_DBG_ONLY(Serial.println(F("Invalid values, retrying")));
    return;
  }

  lecturas.humedadSuelo = ((analogRead(A1)/1023.0) * 100);
  lecturas.luminosidad = ((1023 - analogRead(A0)/1023.0) * 100);
  current_state = states::sending;
}

void sendReadings(ESP8266& wifimod) {
  if(wifimod.createTCP(wifiparams::server, wifiparams::port)) {
    {
      char requestbuf[128];
      sprintf_P(requestbuf, 128, wifiparams::requestFormat, 1, wifiparams::accName,
        lecturas.humedadSuelo, lecturas.humedadAire, lecturas.luminosidad, lecturas.temperatura);

      if(!wifimod.send((const uint8_t*)requestbuf, strlen(requestbuf))) {
        RUN_DBG_ONLY(Serial.println(F("Error while sending data")));
        RUN_DBG_ONLY(Serial.println(F("Retrying to configure connection")));
        current_state = states::searching;
        goto close;
      }
    }

    {
      char recvbuf[160];
      if(wifimod.recv(recvbuf, 160, 10000) <= 0) {
        RUN_DBG_ONLY(Serial.println(F("Error while receiving data")));
        RUN_DBG_ONLY(Serial.println(F("Retrying to configure connection")));
        current_state = states::searching;
        goto close;
      }
      auto jsonit = recvbuf;
      for(; *jsonit != '{'; jsonit++);
      if(!*jsonit) {
        RUN_DBG_ONLY(Serial.println(F("No body has been found in the response, trying again")));
        goto close;
      }

      int orden = 0;
      sscanf(jsonit, "{\"ok\": %d}", &orden);

      if(orden == 1)
        current_state = states::active;
      else
        current_state = states::waiting;
      goto close;
    }
  }
  else {
    RUN_DBG_ONLY(Serial.println(F("Couldn't establish a connection with the server")));
    RUN_DBG_ONLY(Serial.println(F("Retrying to configure connection")));
    current_state = states::searching;
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
  current_state = states::reading;
}

void activateBomb() {
  RUN_DBG_ONLY(Serial.println(F("Activating the water bomb")));
  digitalWrite(pinBomba, HIGH);
  auto time = millis();
  while((millis() - time) <= 10000);
  digitalWrite(pinBomba, LOW);

  RUN_DBG_ONLY(Serial.println(F("Done watering, reading data")));
  current_state = states::reading;
}