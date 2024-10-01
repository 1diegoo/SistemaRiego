#include <DHT.h>
#include "tipos.h"
#include "funcs.h"

namespace gbvars {
  const int DHTTYPE = DHT22;
  constexpr int dhtpin = 9;

  DHT dht(dhtpin, DHTTYPE);
  Lecturas lec = {0};
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(gbconst::pinBomba, OUTPUT);
  gbvars::dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  switch(estadoActual) {
    case Estados::leyendo:
      leerDatos(gbvars::dht, gbvars::lec);
      break;
    case Estados::interpretando:
      interpretarDatos(gbvars::lec);
      break;
    case Estados::ejecutando:
      activarBomba();
  }
}
