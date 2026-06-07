#include <Arduino.h>

#define ADC_PIN 4
#define LED_PIN 15

#define VREF 3.14f
#define ADC_RESOLUTION 4095.f

#define POLLING_INTERVAL 100

void setup() {
    Serial.begin(115200);
    analogReadResolution(12);
}

void loop() {
    const uint16_t adc_value = analogRead(ADC_PIN);
    const float adc_voltage = analogReadMilliVolts(ADC_PIN) / 1000.f;
    const float calc_voltage = (adc_value / ADC_RESOLUTION) * VREF;
    const float error = abs(adc_voltage - calc_voltage) / VREF * 100.f;

    Serial.printf("ADC Value: %u, ADC Voltage: %.2f mV, Calculated Voltage: %.2f V, Error: %.2f%%\n", adc_value, adc_voltage, calc_voltage, error);

    delay(POLLING_INTERVAL);
}
