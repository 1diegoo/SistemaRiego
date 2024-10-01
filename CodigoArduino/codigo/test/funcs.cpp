#include <Arduino.h>
#include <DHT.h>
#include "funcs.h"  |   
#include "tipos.h"

void leerDatos(DHT& dht, Lecturas& lecs) {
  /*lecs.humedadAire = dht.readHumidity();
  Serial.println(lecs.humedadAire);
  lecs.temperatura = dht.readTemperature();
  Serial.println(lecs.temperatura);
  delay(3000);
  if(isnan(lecs.humedadAire) || isnan(lecs.temperatura)) {
    Serial.println("Error en la deteccion de medidas, el sensor respondio con NaN");
    return;
  }*/

  lecs.humedadSuelo = ((1023 - analogRead(A1))/1023.0) * 100;
  Serial.println(lecs.humedadSuelo);
  lecs.luminosidad = (((analogRead(A0))/1023.0) * 100);
  Serial.println(lecs.luminosidad);
  delay(3000);
  estadoActual = Estados::interpretando;
}

void interpretarDatos(Lecturas& lecs) {
  if(lecs.humedadSuelo < 50.0
     //&& lecs.temperatura < 25
     && lecs.luminosidad < 50) {
    Serial.println("Activando riego...");
    estadoActual = Estados::ejecutando;
  }
  else {
    estadoActual = Estados::leyendo;
  }
}

void activarBomba() {
   digitalWrite(gbconst::pinBomba, HIGH);
   delay(10000);
   digitalWrite(gbconst::pinBomba, LOW);
   estadoActual = Estados::leyendo;
}