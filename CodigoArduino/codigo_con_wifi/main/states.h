/**
 * @file states.h
 * Define los estados del programa junto con otros tipos
 * y variables
 */

#ifndef TYPES_H_PROY8
#define TYPES_H_PROY8

#include <stdint.h>
#include <stddef.h>

/**
 * Una enumeración con tipo interno "uint8_t" (un byte) que
 * es una representación de los estados en los que puede
 * estar el accionador.
 */
enum class states : uint8_t{
  searching, ///< El accionador intenta conectarse a una red Wi-Fi
  reading, ///< El accionador realiza una lectura de su entorno
  sending, ///< El accionador se conecta al servidor, envía lecturas y procesa una respuesta
  waiting, ///< El accionador espera 10 segundos
  active ///< El accionador activa el relé conectado a la bomba por 10 segundos
};

/**
 * Esta estructura almacena las lecturas obtenidas en el estado
 * "reading". Cada uno de los valores (excepto temperatura) se
 * encuentra en un rango de entre 0 a 100
 */
struct AccInfo {
  float humedadSuelo; ///< La humedad del suelo (FC-28)
  float humedadAire; ///< La humedad del aire (DHT22)
  float temperatura; ///< La temperatura, en grados Celsius (DHT22)
  float luminosidad; ///< La luminosidad ambiental (LDR)
};

/// @cond undocumented
template <size_t Length>
union ByteField {
  char text[Length];
  uint8_t bytes[Length];
};
/// @endcond

/// Variable que contiene el estado actual del sistema
extern states current_state;

/// Variable que contiene los valores de la última lectura ambiental
extern AccInfo lecturas;

/// Usado para definir el pin del relé (11)
constexpr uint8_t pinBomba = 11;

#endif