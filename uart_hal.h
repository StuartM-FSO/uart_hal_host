#ifndef UART_HAL_H
#define UART_HAL_H

#include <Arduino.h>
#include <stdint.h>

typedef enum{
  COM_ZERO_COUNT = 0U,
  COM_TYPE_HOST,
  COM_TYPE_CLIENT,
  COM_END_COUNT
} comms_system_type_t;

typedef enum{
  COMMS_OK,
  COMMS_INVALID_PARAMETER,
  COMMS_UNINITIALISED
} comms_state_t;

comms_state_t comms_init(const comms_system_type_t system_type);
comms_state_t comms_check(void);

#endif