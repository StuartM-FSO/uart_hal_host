#include "uart_hal.h"


typedef enum{
  COMSTATE_ZERO_COUNT = 0U,
  COMSTATE_HOLD,
  COMSTATE_END_COUNT
} fsm_comstate_t;

typedef struct{
  bool initialised;
  comms_system_type_t system_type;
  fsm_comstate_t internal_fsm_state;
} comms_internal_state_t;


static comms_internal_state_t state = {};

static void fsm_comstate_hold(void);

// Public API

comms_state_t comms_init(const comms_system_type_t system_type){
  if(state.initialised){
    return COMMS_OK;
  }
  if((system_type <= COM_ZERO_COUNT) || (system_type >= COM_END_COUNT)){
    return COMMS_INVALID_PARAMETER;
  }
  state.system_type = system_type;
  state.internal_fsm_state = COMSTATE_HOLD;
  state.initialised = true;
  return COMMS_OK;
}

comms_state_t comms_handle_state(void){
  fsm_comstate_t internal_fsm_state = state.internal_fsm_state;

  switch (internal_fsm_state) {
    case COMSTATE_HOLD:
      fsm_comstate_hold();
      break;
    default:
      break;
  }
  return COMMS_OK;
}

// Private

static void fsm_comstate_hold(void){
  Serial.print("Comstate static");
}