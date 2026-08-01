#include <sys/_stdint.h>
#ifndef COMMS_PROTOCOL_H
#define COMMS_PROTOCOL_H

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
  COMMS_UNINITIALISED,
  COMMS_SERIAL1_FAILED_INIT,
  COMMS_FAILED
} comms_return_t;

comms_return_t comms_init(const comms_system_type_t system_type);
comms_return_t comms_check(void);
comms_return_t comms_handshake(void);
comms_return_t comms_data_packet_request(void);
comms_return_t comms_load_data_packet(uint16_t controller_cell_ppo2_X1000[]);

#endif