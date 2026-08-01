#include "comms_protocol.h"
#include "time_helpers.h"

constexpr uint32_t LOOP_SPEED_MS = 1000U;

typedef enum{
  FSM_UNITIALISED = 0,
  FSM_WAITING
} fsm_state_t;

typedef struct{
  uint32_t loop_timer;
  bool led_on;
  fsm_state_t current_state;
} loop_state_t;

loop_state_t state = {};

bool run_once = false;

void setup() {
  Serial.begin(115200);
  while(!Serial){
    delay(1);
  }
  Serial.println("Host starting");
  pinMode(LED_BUILTIN, OUTPUT);

  if(comms_init(COM_TYPE_HOST) != COMMS_OK){
    Serial.println("Init failed");
    for(;;);
  }
  state.loop_timer = millis();
  state.led_on = false;
  state.current_state = FSM_WAITING;
  Serial.println("comms init as host");
  load_payload();
}

void loop() {
  uint32_t now = millis();
  uint32_t loop_timer = state.loop_timer;
  bool led_on = state.led_on;
  fsm_state_t current_state = state.current_state;

  switch (current_state) {
    case FSM_WAITING:
      fsm_waiting();
      break;
    default:
      break;
  }

  if(has_timer_elapsed(now, loop_timer, LOOP_SPEED_MS)){
    state.loop_timer = now;
    digitalWrite(LED_BUILTIN, led_on);
    state.led_on = !led_on;
  }
}

////////////

void fsm_waiting(void){
  uint32_t now = millis();
  uint32_t loop_timer = state.loop_timer;

  comms_check();
  
}

void load_payload(){
  uint16_t cells[3] = {};

  cells[0] = 1111U;
  cells[1] = 2222U;
  cells[2] = 3333U;

}