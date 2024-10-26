/**
 * @file funcs.h
 * Contiene declaraciones para las funciones utilizadas en
 * la máquina de estados del programa
 */

#ifndef FUNCS_H_PROY8
#define FUNCS_H_PROY8

#include <ESP8266.h>

/**
 * Intenta realizar una conexión a una red Wi-Fi, luego,
 * cambia el estado a "reading".
 *
 * connect() se ejecuta cuando el valor de "current_state"
 * es "searching", luego, busca una red con SSID "RedComun"
 * y contraseña "admin123". Si falla en realizar la conexión
 * mantiene el estado y vuelve a ejecutarse
 *
 * @param[in] wificon una instancia de ESP8266 que conecta con el ESP-01
 */
void connect(ESP8266& wificon) noexcept;

/**
 * Obtiene lecturas del ambiente y cambia el estado a "sending"
 * 
 * readData() se ejecuta cuando el valor de "current_state"
 * es "reading",si readData() recibe valores NaN del DHT22
 * mantiene el estado y vuelve a leer.
 *
 * @param[in] sensor una instancia de DHT que conecta con el DHT22
 */
void readData(DHT& sensor);

/**
 * Envía las últimas lecturas registradas en "lecturas" al servidor y cambia
 * de estado según la respuesta del servidor
 *
 * sendReadings() se ejecuta cuando el valor de "current_state" es "sending"
 * e intenta conectarse a la dirección 10.0.0.2 en el puerto 5255 por medio
 * de una conexión TCP, luego envía los resultados en un método GET al servidor
 * para finalmente, recibir un a respuesta json en formato {"ok": 1|0}.
 * los parámetros están codificados de la siguiente manera:
 * "Id=%d&Nm=%S&Sh=%.1f&Ah=%.1f&Tm=%.1f&Lm=%.1f".
 *
 * Si el valor de "ok" en la respuesta es 1, el valor de "current_state" pasa
 * a ser "active", caso contrario, pasa a ser "sleeping"
 * 
 * @param[in] wifimod una instancia de ESP8266 que conecta con el ESP-01
 */
void sendReadings(ESP8266& wifimod);

/**
 * Detiene el accionador por 10 segundos y cambia de estado a "reading"
 *
 * setOnSleepMode() se ejecuta cuando el valor de current_state es "sleeping".
 * Durante este tiempo, el accionador no hace ninguna acción.
 */
void setOnSleepMode();

/**
 * Activa la bomba de agua por 10 segundos y cambia de estado a "reading"
 *
 * activateBomb() se ejecuta cuando el valor de current_state es "active"
 * y activa el pin 11 (el pin que dispara el relé) para encender la bomba
 * agua, luego, lo mantiene activo por 10 segundos para luego desactivar
 * el pin. Durante este tiempo, el accionador no realizará ninguna otra
 * acción.
 */
void activateBomb();

#endif