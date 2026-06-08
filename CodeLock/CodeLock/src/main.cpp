#include <Arduino.h>

#define BUTTON_PIN_A 0
#define BUTTON_PIN_B 17

#define LED_PIN_GREEN 15
#define LED_PIN_RED 16
#define LED_BLINK_NUM 3
#define LED_BLINK_DELAY 500

#define POLLING_DELAY 100

#define CODE_LENGTH 4

enum class LockStateMachine : uint8_t {
    Input,
    Verify,
    Success,
    Error
};

static LockStateMachine _current_lock_state;
static uint8_t _current_input_num;
static char _input_buffer[CODE_LENGTH + 1];
static bool _buffer_updated;

const char _lock_code[CODE_LENGTH + 1] = "ABBA";

static bool isButtonPressed(const uint8_t pin) {
    assert(pin == BUTTON_PIN_A || pin == BUTTON_PIN_B);
    return pin == BUTTON_PIN_A ? digitalRead(BUTTON_PIN_A) == LOW : digitalRead(BUTTON_PIN_B) == HIGH;
}

static void resetAll() {
    digitalWrite(LED_PIN_GREEN, LOW);
    digitalWrite(LED_PIN_RED, LOW);

    _current_lock_state = LockStateMachine::Input;
    _current_input_num = 0;
    _buffer_updated = false;
}

static void processLEDBlink(const uint8_t pin) {
    uint8_t current_led_value = LOW;
    for (uint8_t i = 0; i < LED_BLINK_NUM * 2; ++i) {
        current_led_value = !current_led_value;
        digitalWrite(pin, current_led_value);
        delay(LED_BLINK_DELAY);
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(BUTTON_PIN_A, INPUT_PULLUP);
    pinMode(BUTTON_PIN_B, INPUT_PULLUP);
    pinMode(LED_PIN_GREEN, OUTPUT);
    pinMode(LED_PIN_RED, OUTPUT);

    resetAll();
}

void loop() {
    switch (_current_lock_state) {
        case LockStateMachine::Input: {
            const bool bOneButtonPressed = isButtonPressed(BUTTON_PIN_A) ^ isButtonPressed(BUTTON_PIN_B);
            const bool bAllButtonsUnpressed = !isButtonPressed(BUTTON_PIN_A) && !isButtonPressed(BUTTON_PIN_B);

            if (bAllButtonsUnpressed) {
                _buffer_updated = false;
                break;
            }
            else if (!bOneButtonPressed || _buffer_updated) {
                break;
            }

            _input_buffer[_current_input_num++] = isButtonPressed(BUTTON_PIN_A) ? 'A' : 'B';
            _buffer_updated = true;

            if (_current_input_num == CODE_LENGTH) {
                _input_buffer[_current_input_num] = '\0';
                _current_lock_state = LockStateMachine::Verify;
            }

            Serial.printf("Saving [%d:%c] input\n", _current_input_num - 1, _input_buffer[_current_input_num - 1]);

            break;
        }
        case LockStateMachine::Verify: {
            Serial.printf("Verifying input buffer\n");

            _current_lock_state = strcmp_P(_input_buffer, _lock_code) ? LockStateMachine::Error : LockStateMachine::Success;
            break;
        }
        case LockStateMachine::Success: {
            Serial.printf("Lock is unlocked\n");
            processLEDBlink(LED_PIN_GREEN);
            resetAll();

            break;
        }
        case LockStateMachine::Error: {
            Serial.printf("Lock is locked\n");
            processLEDBlink(LED_PIN_RED);
            resetAll();

            break;
        }
    }

    delay(POLLING_DELAY);
}
