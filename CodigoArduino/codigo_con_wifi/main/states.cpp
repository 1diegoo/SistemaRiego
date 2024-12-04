#include "states.h"

states currentState = states::searching;
AccInfo readings = { 0 };

states switchState(states newState) {
  digitalWrite((uint8_t)currentState, LOW);
  auto tmp = currentState;
  currentState = newState;
  digitalWrite((uint8_t)currentState, HIGH);
  return tmp;
}