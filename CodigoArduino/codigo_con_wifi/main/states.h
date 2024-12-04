/**
 * @file states.h
 * Define los estados del programa junto con otros tipos
 * y variables
 */

#ifndef TYPES_H_PROY8
#define TYPES_H_PROY8

#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>

/**
 * Una enumeración con tipo interno "uint8_t" (un byte) que
 * es una representación de los estados en los que puede
 * estar el accionador.
 */
enum class states : uint8_t{
  searching = 3, ///< El accionador intenta conectarse a una red Wi-Fi
  reading, ///< El accionador realiza una lectura de su entorno
  sending, ///< El accionador se conecta al servidor, envía lecturas y procesa una respuesta
  waiting, ///< El accionador espera 10 segundos
  active ///< El accionador activa el relé conectado a la bomba por 10 segundos
};

/**
 * Cambia el estado del programa a newState.
 * También enciende un LED indicador del estado actual
 * 
 * @param[in] newState el nuevo estado
 * @return el estado previo
 */
states switchState(states newState) noexcept;

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
extern states currentState;

/// Variable que contiene los valores de la última lectura ambiental
extern AccInfo readings;

/**
 * Este espacio de nombres contiene constantes que representan
 * los pines utilizados por el programa
 */
namespace pins {
  constexpr uint8_t espRx = 4; ///< El pin de recepción de datos desde el ESP-01
  constexpr uint8_t espTx = 5; ///< El pin de transmisión de datos hacia el ESP-01
  constexpr uint8_t pump = 11; ///< El pin de la bomba de agua (en realidad, es el pin del relé que conecta a la bomba de agua)
  constexpr uint8_t dht = 9; ///< El pin de recepción de datos desde el DHT-22
  constexpr uint8_t fc28 = A1; ///< El pin de recepción de datos desde el FC-28
  constexpr uint8_t ldr = A0; ///< El pin de recepción de señales del fotorresistor
  constexpr uint8_t actionStatus = 13; ///< El pin para indicar el estado de la acción actual
}

#endif