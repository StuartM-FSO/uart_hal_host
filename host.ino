#include "uart_hal.h"
#include "time_helpers.h"

constexpr uint32_t LOOP_SPEED_MS = 1000U;

typedef struct{
  uint32_t loop_timer;
  bool led_on;
} loop_state_t;

loop_state_t state = {};

void setup() {
  Serial.begin(115200);
  while(!Serial){
    delay(1);
  }
  Serial.println("Host starting");
  pinMode(LED_BUILTIN, OUTPUT);

  comms_init(COM_TYPE_HOST);
  state.loop_timer = millis();
  state.led_on = false;
  Serial.println("comms init as host");
}

void loop() {
  uint32_t now = millis();
  uint32_t loop_timer = state.loop_timer;
  bool led_on = state.led_on;

  if(has_timer_elapsed(now, loop_timer, LOOP_SPEED_MS)){
    Serial.println("+");
    state.loop_timer = now;
    digitalWrite(LED_BUILTIN, led_on);
    state.led_on = !led_on;
  }
}
