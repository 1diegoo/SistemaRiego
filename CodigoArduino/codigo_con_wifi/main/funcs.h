#ifndef FUNCS_H_PROY8
#define FUNCS_H_PROY8

#include <ESP8266.h>

void connect(ESP8266& wificon) noexcept;

void readData(DHT& sensor);

void sendReadings(ESP8266& wifimod);

void setOnSleepMode();

void activateBomb();

#endif