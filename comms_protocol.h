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
comms_return_t comms_prepare_payload(const uint16_t * ppo2_x1000);

comms_return_t comms_get_ppo2_x1000(const uint8_t channel, uint16_t * const ppo2_x1000);
comms_return_t comms_get_latest_id(uint32_t * const latest_id);

bool comms_payload_updated(void);
#endif