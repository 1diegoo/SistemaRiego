#include <DHT.h>

/*#define USE_SOFTSERIAL

#ifdef USE_SOFTSERIAL
#define ESP8266_USE_SOFTWARE_SERIAL
#include <SoftwareSerial.h>
#endif*/
#define DEBUG_MODE

#include <ESP8266.h>
#include <SoftwareSerial.h>

#include "dbg_utils.h"
#include "states.h"
#include "funcs.h"
 
/*#ifdef USE_SOFTSERIAL
SoftwareSerial espserial(9, 8); // RX = 9 && TX = 8
#else
auto& espserial = Serial;
#endif*/
namespace gbvars {
  constexpr int DHTTYPE = DHT22;
  DHT dht(9, DHTTYPE);
  SoftwareSerial espserial(2, 3);
  ESP8266 wifimod(espserial);
}

void setup() {
  // put your setup code here, to run once:
  /*#ifdef USE_SOFTSERIAL*/
  Serial.begin(9600);
  /*#endif*/
  pinMode(pinBomba, OUTPUT);
  while(!gbvars::wifimod.setOprToStationSoftAP()){
    RUN_DBG_ONLY(Serial.println(F("Attempting to setup in station + softap mode...")));
    delay(1000);
  }
  gbvars::dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  switch(current_state) {
    case states::searching:
      connect(gbvars::wifimod);
      break;
    case states::reading:
      readData(gbvars::dht);
      break;
    case states::sending:
      sendReadings(gbvars::wifimod);
      break;
    case states::waiting:
      setOnSleepMode();
      break;
    case states::active:
      activateBomb();
  }
}
