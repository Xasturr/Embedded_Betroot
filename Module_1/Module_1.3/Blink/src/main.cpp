#include <Arduino.h>

#define RED_LED_PIN 4
#define BLUE_LED_PIN 5

static uint8_t _value = 0;
static uint32_t _delay = 500;
static int8_t _delay_delta = 64;

void setup() {
  Serial.begin(115200);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
}

void loop() {
  _value = !_value;
  digitalWrite(RED_LED_PIN, _value);
  digitalWrite(BLUE_LED_PIN, !_value);
  delay(_delay);
  
  if (_delay > 500 || _delay < 100)
  {
    _delay_delta = -_delay_delta;
  }
  
  _delay += _delay_delta;
}
