#ifndef SERIAL1_HAL_H
#define SERIAL1_HAL_H

#include <Arduino.h>
#include <stdint.h>

typedef enum{
  SER_OK,
  SER_UNITIIALISED,
  SER_INVALID_PARAMETER
} serial_state_t;

serial_state_t serial1_init();

#endif