/**
 * @file main.ino
 * El punto de partida del programa
 * 
 * para la implementación de este código, se utilizaron las siguientes
 * dependencias:
 *
 * -> DHT sensor library (<DHT.h>):
 *    Para el uso del sensor de humedad atmosférica y temperatura DHT22
 * -> ITEADLIB Arduino WeeESP8266 (<ESP8266.h>):
 *    Define una interfaz para la comunicación con el ESP-01. Utilizado
 *    para las comunicaciones de red a través de Wi-Fi
 * -> Software Serial (<SoftwareSerial.h>):
 *    Define una interfaz serie virtual. Usado para habilitar un medio
 *    de comunicación entre el Arduino y el ESP-01
 */
#include <DHT.h>

#define DEBUG_MODE

#include <ESP8266.h>
#include <SoftwareSerial.h>

#include "dbg_utils.h"
#include "states.h"
#include "funcs.h"

/**
 * Este espacio de nombre declara una serie de variables globales
 * en el programa
 */
namespace gbvars {
  const int DHTTYPE = DHT22;
  DHT dht(9, DHTTYPE);
  SoftwareSerial espserial(2, 3);
  ESP8266 wifimod(espserial);
}

/**
 * setup() se ejecuta al inicio del programa, activa el puerto
 * serie a 9600 baudios para depuración, configura el pin del relé
 * y el módulo Wi-Fi.
 */
void setup() {
  Serial.begin(9600);
  pinMode(pinBomba, OUTPUT);
  while(!gbvars::wifimod.setOprToStationSoftAP()){
    RUN_DBG_ONLY(Serial.println(F("Attempting to setup in station + softap mode...")));
    delay(1000);
  }
  gbvars::dht.begin();
}

/**
 * loop() es el corazón del programa, incluye una máquina de estados
 * para determinar las acciones que debería realizar el accionador
 *
 * Más detalle en funcs.h (funciones) y states.h (estados)
 * Estado -> función llamada
 * searching -> connect(1)
 * reading -> readData(1)
 * sending -> sendReadings(1)
 * waiting -> setOnSleepMode()
 * active -> activateBomb()
 */
void loop() {
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
