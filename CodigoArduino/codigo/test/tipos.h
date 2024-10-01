#ifndef TIPOS_H_TEST8
#define TIPOS_H_TEST8

#include <stdint.h>

namespace gbconst {
  constexpr int pinBomba = 11;
};

enum class Estados : uint8_t {
  leyendo,
  interpretando,
  ejecutando
};

struct Lecturas {
  float humedadSuelo;
  float humedadAire;
  float temperatura;
  float luminosidad;
};

extern Estados estadoActual;

#endif