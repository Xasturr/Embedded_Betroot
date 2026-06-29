#include <Arduino.h>

#define BUTTON_PIN 17
#define BOOT_PIN 0
#define UPDATE_INTERVAL 1000
#define SEGMENTS_NUM 7

const uint8_t segmentPins[] = {1, 2, 3, 4, 5, 6, 7};
const uint8_t digitPins[] = {10, 11};

const uint8_t digits[10][SEGMENTS_NUM] = {
    {0, 0, 0, 0, 0, 0, 1}, // 0
    {1, 0, 0, 1, 1, 1, 1}, // 1
    {0, 0, 1, 0, 0, 1, 0}, // 2
    {0, 0, 0, 0, 1, 1, 0}, // 3
    {1, 0, 0, 1, 1, 0, 0}, // 4
    {0, 1, 0, 0, 1, 0, 0}, // 5
    {0, 1, 0, 0, 0, 0, 0}, // 6
    {0, 0, 0, 1, 1, 1, 1}, // 7
    {0, 0, 0, 0, 0, 0, 0}, // 8
    {0, 0, 0, 0, 1, 0, 0}  // 9
};

unsigned long lastUpdateTime = 0;
unsigned long dtBeforePause = 0;
int counter = 0;

int tens = 0;
int ones = 0;

volatile bool bTimerActive = true;
volatile unsigned long lastButtonInterruptTime = 0;
volatile unsigned long lastBootInterruptTime = 0;

void IRAM_ATTR handleButtonPress() {
    unsigned long currentTime = millis();

    if (currentTime - lastButtonInterruptTime < 50) {
        return;
    }

    lastButtonInterruptTime = currentTime;
    bTimerActive = !bTimerActive;

    if (bTimerActive) {
        lastUpdateTime = currentTime;
    }
    else {
        dtBeforePause = currentTime - lastUpdateTime;
    }
}

void IRAM_ATTR handleBootPress() {
    unsigned long currentTime = millis();

    if (currentTime - lastBootInterruptTime < 50) {
        return;
    }

    lastBootInterruptTime = currentTime;
    counter = 0;
    bTimerActive = false;
}

void setSegments(int number) {
    for (int i = 0; i < SEGMENTS_NUM; i++) {
        digitalWrite(segmentPins[i], digits[number][i]);
    }
}

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BOOT_PIN, INPUT_PULLUP);

    for (int i = 0; i < SEGMENTS_NUM; i++) {
        pinMode(segmentPins[i], OUTPUT);
        digitalWrite(segmentPins[i], HIGH);
    }
    
    for (int i = 0; i < 2; i++) {
        pinMode(digitPins[i], OUTPUT);
        digitalWrite(digitPins[i], LOW);
    }

    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);
    attachInterrupt(digitalPinToInterrupt(BOOT_PIN), handleBootPress, FALLING);
}

void loop() {
    const unsigned long currentTime = millis();

    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL - dtBeforePause && bTimerActive) {
        dtBeforePause = 0;
        lastUpdateTime = currentTime;
        ++counter;

        if (counter > 99)
        {
            counter = 0;
        }
    }

    tens = counter / 10;
    ones = counter % 10;

    setSegments(tens);
    digitalWrite(digitPins[0], HIGH);
    delay(5);
    digitalWrite(digitPins[0], LOW);

    setSegments(ones);
    digitalWrite(digitPins[1], HIGH);
    delay(5);
    digitalWrite(digitPins[1], LOW);
}