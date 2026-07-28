#include <Arduino.h>
#include <stdint.h>
#include "comms_protocol.h"
#include "time_helpers.h"
#include "serial1_hal.h"

constexpr uint32_t MAX_ACK_WAIT_MS = 2000U;

typedef enum{
  COMSTATE_ZERO_COUNT = 0U,
  COMSTATE_UNITIALISED,
  COMSTATE_LISTEN,
  COMSTATE_SEND_HANDSHAKE,
  COMSTATE_WAIT_FOR_ACKNOWLEDGEMENT,
  COMSTATE_ACKNOWLEDGE_HANDSHAKE,
  COMSTATE_DEBUG_SEQUENCE_END,
  COMSTATE_TIMEOUT,
  COMSTATE_END_COUNT
} comstate_t;

typedef enum{
  REMOTE_UNINITIALISED,
  REMOTE_CONNECTED,
  REMOTE_NOT_CONNECTED
} remote_t;

typedef struct{
  bool initialised;
  bool handshake_timer_running;
  comms_system_type_t system_type;
  comstate_t internal_state;
  uint32_t ack_wait_timer_ms;
  remote_t remote_device_state;
} comms_internal_state_t;


static comms_internal_state_t state = {};


// Private function declarations
//    FSM
static void comstate_listen(void);
static void comstate_wait_for_acknowledgement(void);
static void comstate_send_handshake(void);
static void comstate_timeout(void);
static void comstate_acknowledge_handshake(void);

static void comstate_debug_sequence_end(void);

//    General
static void state_transition(comstate_t state);
static comstate_t process_command(const tx_command_t received_command);

// Public API

comms_return_t comms_init(const comms_system_type_t system_type){
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
  state.internal_state = COMSTATE_LISTEN;
  state.ack_wait_timer_ms = 0U;
  state.remote_device_state = REMOTE_UNINITIALISED;
  state.handshake_timer_running = false;
  state.initialised = true;
  return COMMS_OK;
}

comms_return_t comms_check(void){

  switch (state.internal_state) {
    case COMSTATE_LISTEN:
      comstate_listen();
      break;
    case COMSTATE_WAIT_FOR_ACKNOWLEDGEMENT:
      comstate_wait_for_acknowledgement();
      break;
    case COMSTATE_ACKNOWLEDGE_HANDSHAKE:
      comstate_acknowledge_handshake();
      break;
    case COMSTATE_SEND_HANDSHAKE:
      comstate_send_handshake();
      break;
    case COMSTATE_TIMEOUT:
      comstate_timeout();
      break;
    case COMSTATE_DEBUG_SEQUENCE_END:
      comstate_debug_sequence_end();
    default:
      break;
  }
  return COMMS_OK;
}

comms_return_t comms_handshake(void){
  state_transition(COMSTATE_SEND_HANDSHAKE);
  return COMMS_OK;
}

// Private

static comstate_t process_command(const tx_command_t received_command){
  comstate_t transition_to = COMSTATE_UNITIALISED;

  switch (received_command) {
    case TX_HANDSHAKE_REQUEST:
      Serial.println("Command processed - TX_HANDSHAKE_REQUEST");
      transition_to = COMSTATE_ACKNOWLEDGE_HANDSHAKE;
      break;
    case TX_HANDSHAKE_ACKNOWLEDGED:
      Serial.println("Command processed - TX_HANDSHAKE_ACKNOWLEDGED");
      transition_to = COMSTATE_LISTEN;
      break;
    default:
      break;
  }
  return transition_to;
}

//    FSM States

static void state_transition(comstate_t new_state){
  state.internal_state = new_state;
}

static void comstate_listen(void){
  tx_command_t command = TX_UNINITIALISED;
  serial_state_t result = serial1_listen_for_command(&command);
  comstate_t transition_to = COMSTATE_UNITIALISED;

  if(result == SER_OK){
    Serial.print("Command received: ");
    Serial.println(command);
    // Check if command is valid
    transition_to = process_command(command);
    state_transition(transition_to);
  }
  if((result == SER_INVALID_PARAMETER) || (result == SER_UNITIIALISED)){
    // Handle error
  }
  if(result == SER_NOTHING_SENT){

  }
}

static void comstate_wait_for_acknowledgement(void){
  uint32_t now = millis();
  uint32_t ack_wait_timer_ms = state.ack_wait_timer_ms;
  tx_command_t command = TX_UNINITIALISED;
  serial_state_t result = serial1_listen_for_command(&command);
  comstate_t next_state_transition = COMSTATE_UNITIALISED;

  if(result == SER_OK){
    next_state_transition = process_command(command);
    state.remote_device_state = REMOTE_CONNECTED;
    Serial.println("Handshake acknowledgement received");
  }

  if(result == SER_NOTHING_SENT){
    next_state_transition = COMSTATE_WAIT_FOR_ACKNOWLEDGEMENT;
  }

  if((result == SER_INVALID_PARAMETER) || (result == SER_UNITIIALISED)){
    Serial.println("Failed");
    state.remote_device_state = REMOTE_NOT_CONNECTED;
    // Handle errors here
  }

  if(has_timer_elapsed(now, ack_wait_timer_ms, MAX_ACK_WAIT_MS)){
    Serial.println("Timer expired");
    state_transition(COMSTATE_TIMEOUT);
    state.remote_device_state = REMOTE_NOT_CONNECTED;
    state.handshake_timer_running = false;
    return;
  }

  state_transition(next_state_transition);
}

static void comstate_acknowledge_handshake(void){
  Serial.println("Handshake acknowledged");
  serial1_send_command(TX_HANDSHAKE_ACKNOWLEDGED);
  state_transition(COMSTATE_LISTEN);
}

static void comstate_send_handshake(void){
  if(!state.handshake_timer_running){
    Serial.println("Handshake sent");
    serial1_send_command(TX_HANDSHAKE_REQUEST);
    state.ack_wait_timer_ms = millis();
    state.remote_device_state = REMOTE_UNINITIALISED;
    state.handshake_timer_running = true;  
  } else {
    Serial.println("Handshake timer already running");
  }
  state_transition(COMSTATE_WAIT_FOR_ACKNOWLEDGEMENT);
}

static void comstate_timeout(void){
  Serial.println("Timed out");
  state_transition(COMSTATE_LISTEN);
}






static void comstate_debug_sequence_end(void){
  Serial.println("Sequence end");
  for(;;);
}