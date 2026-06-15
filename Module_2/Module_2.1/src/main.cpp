#include <Arduino.h>

enum class LedState  : uint8_t {
    Off = 0,
    On
};

class Led {
public:
    void init() {
        pinMode(pin_, OUTPUT);

        ledState_ = LedState::Off;
        digitalWrite(pin_, (uint8_t)ledState_);

        changeStateTime_ = 0;
    }

    void set(const LedState ledState) {
        if (ledState_ == ledState) {
            return;
        }

        const unsigned long Time = millis();
        if (Time - changeStateTime_ < blinkTimeMS_) {
            return;
        }

        ledState_ = ledState;
        digitalWrite(pin_, (uint8_t)ledState_);

        changeStateTime_ = Time;
    }

    inline LedState getState() const {
        return ledState_;
    }

private:
    LedState ledState_;

    const uint8_t pin_ = 16;
    const uint16_t blinkTimeMS_ = 500;

    unsigned long changeStateTime_;
};

Led led;

void setup() {
    Serial.begin(115200);

    led.init();
}

void loop() {
    const LedState desiredState = (LedState)!bool(led.getState()); 
    led.set(desiredState);
}
