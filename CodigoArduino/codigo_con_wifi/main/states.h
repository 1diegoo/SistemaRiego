#ifndef TYPES_H_PROY8
#define TYPES_H_PROY8

#include <stdint.h>
#include <stddef.h>

enum class states : uint8_t{
  searching,
  reading,
  sending,
  waiting,
  active
};

struct AccInfo {
  float humedadSuelo;
  float humedadAire;
  float temperatura;
  float luminosidad;
};

template <size_t Length>
union ByteField {
  char text[Length];
  uint8_t bytes[Length];
};

extern states current_state;

extern AccInfo lecturas;

constexpr uint8_t pinBomba = 11;

#endif