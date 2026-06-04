#include <Arduino.h>

#define BUTTON_PIN 17
#define BOOT_PIN 0

#define LED_PIN_0 16
#define LED_PIN_1 17

static int32_t _delay_0 = 256;
static int32_t _delay_1 = 1024;
static int32_t _delay_2 = 2048;
static int32_t current_delay = _delay_0;

static uint8_t _led_state = 0;

void setup() {
    Serial.begin(115200);
    delay(1000); 

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BOOT_PIN, INPUT_PULLUP);
    pinMode(LED_PIN_0, OUTPUT);
    pinMode(LED_PIN_1, OUTPUT);
}

void loop() {
    _led_state = !_led_state;

    if (digitalRead(BUTTON_PIN) == HIGH && digitalRead(BOOT_PIN) == LOW) {
        current_delay = _delay_2;
    }
    else if (digitalRead(BUTTON_PIN) == HIGH) {
        current_delay = _delay_0;
    }
    else if (digitalRead(BOOT_PIN) == LOW) {
        current_delay = _delay_1;
    }

    digitalWrite(LED_PIN_0, _led_state);
    digitalWrite(LED_PIN_1, !_led_state);
    
    delay(current_delay);
} 