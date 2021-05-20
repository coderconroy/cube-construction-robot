#include <Arduino.h>

static const uint8_t pressurePin = A7;

void setup() {
    Serial.begin(19200); 
}

void loop() {
    // Read pressure
    int16_t value = analogRead(pressurePin);

    // Transmit pressure bytes via serial
    Serial.write(value>>8 & 0xFF);
    Serial.write(value & 0xFF);

    // Sampling rate approximately 1 Hz
    delay(1000); 
}