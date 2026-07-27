#include "api/Common.h"
#include <sys/_stdint.h>
#include "serial1_hal.h"
#include <Arduino.h>
#include <stdint.h>
#include <SerialTransfer.h>

SerialTransfer comms;

constexpr uint16_t BAUD_RATE = 9600U;

typedef struct{
  bool initialised;
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
  return SER_OK;
}



// Private