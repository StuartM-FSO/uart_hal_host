#include <Arduino.h>
#include <stdint.h>
#include "comms_protocol.h"
#include "time_helpers.h"
#include "serial1_hal.h"

constexpr uint32_t MAX_ACK_WAIT_MS = 5000U;

typedef enum{
  COMSTATE_ZERO_COUNT = 0U,
  COMSTATE_HOLD,
  COMSTATE_SEND_HANDSHAKE,
  COMSTATE_WAIT_FOR_ACKNOWLEDGEMENT,
  COMSTATE_DEBUG_SEQUENCE_END,
  COMSTATE_END_COUNT
} comstate_t;

typedef struct{
  bool initialised;
  comms_system_type_t system_type;
  comstate_t internal_state;
  uint32_t ack_wait_timer_ms;
} comms_internal_state_t;


static comms_internal_state_t state = {};

static void comstate_hold(void);
static void comstate_wait_for_acknowledgement(void);
static void comstate_send_handshake(void);

static void comstate_debug_sequence_end(void);

// Public API

comms_state_t comms_init(const comms_system_type_t system_type){
  if(state.initialised){
    return COMMS_OK;
  }
  if((system_type <= COM_ZERO_COUNT) || (system_type >= COM_END_COUNT)){
    return COMMS_INVALID_PARAMETER;
  }
  if(serial1_init() != SER_OK){
    return COMMS_SERIAL1_FAILED_INIT;
  }
  state.system_type = system_type;
  state.internal_state = COMSTATE_HOLD;
  state.ack_wait_timer_ms = 0U;
  state.initialised = true;
  return COMMS_OK;
}

comms_state_t comms_check(void){

  switch (state.internal_state) {
    case COMSTATE_HOLD:
      comstate_hold();
      break;
    case COMSTATE_WAIT_FOR_ACKNOWLEDGEMENT:
      comstate_wait_for_acknowledgement();
      break;
    case COMSTATE_SEND_HANDSHAKE:
      comstate_send_handshake();
      break;
    case COMSTATE_DEBUG_SEQUENCE_END:
      comstate_debug_sequence_end();
    default:
      break;
  }
  return COMMS_OK;
}

// Private

static void comstate_hold(void){
  Serial.println("Comstate static");
}

static void comstate_wait_for_acknowledgement(void){
  uint32_t now = millis();
  uint32_t ack_wait_timer_ms = state.ack_wait_timer_ms;

  if(has_timer_elapsed(now, ack_wait_timer_ms, MAX_ACK_WAIT_MS)){
    Serial.println("Timer expired");
    state.internal_state = COMSTATE_DEBUG_SEQUENCE_END;
    return;
  }
}

static void comstate_send_handshake(void){
  Serial.println("Handshake sent");
  /* Send command here */
  state.ack_wait_timer_ms = millis();
  state.internal_state = COMSTATE_WAIT_FOR_ACKNOWLEDGEMENT;
}






static void comstate_debug_sequence_end(void){
  Serial.println("Sequence end");
  for(;;);
}