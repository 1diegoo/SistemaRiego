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

//#define DEBUG_MODE

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
  DHT dht(pins::dht, DHTTYPE);
  //SoftwareSerial espserial(pins::espRx, pins::espTx);
  //ESP8266 wifimod(espserial);
  ESP8266 wifimod(Serial, 9600);
}

/**
 * setup() se ejecuta al inicio del programa, activa el puerto
 * serie a 9600 baudios para depuración, configura el pin del relé
 * y el módulo Wi-Fi.
 */
void setup() {
  pinMode(pins::pump, OUTPUT);
  pinMode(pins::actionStatus, OUTPUT);
  /*pinMode(pins::espTx, OUTPUT);
  pinMode(pins::espRx, INPUT);*/

  int statusPins[] = {3, 4, 5, 6, 7};
  for (int pin : statusPins) {
    pinMode(pin, OUTPUT);
  }

  for (int pin : statusPins) {
    digitalWrite(pin, HIGH);
    delay(150);
  }
  for (int pin : statusPins) {
    digitalWrite(pin, LOW);
    delay(150);
  }

  Serial.begin(9600);
  //gbvars::espserial.begin(9600);

  digitalWrite(pins::actionStatus, HIGH);
  while(!gbvars::wifimod.kick()) {
    digitalWrite(pins::actionStatus, LOW);
    delay(200);
    digitalWrite(pins::actionStatus, HIGH);
    delay(200);
    digitalWrite(pins::actionStatus, LOW);
    delay(200);
    digitalWrite(pins::actionStatus, HIGH);
    delay(200);
  }

  while(!gbvars::wifimod.setOprToStationSoftAP()) {
    RUN_DBG_ONLY(Serial.println(F("Attempting to setup in station + softap mode...")));
    //delay(1000);
    digitalWrite(pins::actionStatus, LOW);
    delay(200);
    digitalWrite(pins::actionStatus, HIGH);
    delay(200);
  }
  gbvars::dht.begin();
  digitalWrite(pins::actionStatus, LOW);

  digitalWrite(*statusPins, HIGH); // Enciende el led que indica states::searching
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
  switch(currentState) {
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
