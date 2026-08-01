#include "api/Common.h"
#include <sys/_stdint.h>
#include "serial1_hal.h"
#include <Arduino.h>
#include <stdint.h>
#include <SerialTransfer.h>


constexpr uint16_t BAUD_RATE = 9600U;
constexpr uint8_t THREE_CELLS = 3U;

struct __attribute__((packed)) STRUCT {
  tx_command_t tx_command;
  uint32_t id;
  uint16_t tx_cell[THREE_CELLS];
} testStruct;

SerialTransfer comms;


typedef enum{
  COMM_HANDSHAKE,
  COMM_ACKNOWLEDGED,
  COMM_FAILED
} serial_command_t;

typedef struct{
  bool initialised;
  uint32_t last_packet_id;
} internal_state_t;

static internal_state_t state = {};

// Public API

serial_state_t serial1_init(){
  if(state.initialised){
    return SER_OK;
  }
  Serial1.begin(BAUD_RATE);
  while(!Serial1){
    delay(1);
  }
  comms.begin(Serial1);
  state.initialised = true;
  state.last_packet_id = 0U;
  return SER_OK;
}

serial_state_t serial1_listen_for_command(tx_command_t * const command){
  uint16_t rx_size = 0U;
  tx_command_t rx_command = TX_UNINITIALISED;

  if(command == NULL){
    return SER_INVALID_PARAMETER;
  }
  if(!state.initialised){
    return SER_UNINITIALISED;
  }
  if(!comms.available()){
    return SER_NOTHING_SENT;
  }
  rx_size = comms.rxObj(rx_command, rx_size);
  *command = rx_command;
  return SER_OK;
}

serial_state_t serial1_send_command(const tx_command_t command){
  uint16_t tx_size = 0U;

  tx_size = comms.txObj(command, tx_size);
  comms.sendData(tx_size);
  return SER_OK;
}



// Private