#include <SoftwareSerial.h>

// Define Software Serial pins (RX, TX)
SoftwareSerial mySerial(10, 11); // RX on pin 10, TX on pin 11

// Frame configuration
#define START_BYTE 0xAA
#define END_BYTE 0x55
#define FRAME_SIZE 8

void setup() {
    // Initialize software serial and hardware serial
    mySerial.begin(9600); // Baud rate for SoftwareSerial
    Serial.begin(9600);   // Baud rate for debug output
    Serial.println("Arduino Serial Decoder Started");
}

void loop() {
    uint8_t frame[FRAME_SIZE];

    // Wait for start byte
    if (mySerial.available() > 0 && mySerial.read() == START_BYTE) {
        // Read the rest of the frame
        for (int i = 1; i < FRAME_SIZE; i++) {
            while (!mySerial.available()); // Wait for data
            frame[i] = mySerial.read();
        }

        // Validate the end byte
        if (frame[FRAME_SIZE - 1] == END_BYTE) {
            // Decode the frame
            uint16_t co2 = (frame[1] << 8) | frame[2];
            int16_t temperature = (int16_t)((frame[3] << 8) | frame[4]);
            int16_t humidity = (frame[5] << 8) | frame[6];

            float tempC = temperature / 100.0;
            float humRH = humidity / 100.0;

            // Print the decoded values
            Serial.print("CO2: ");
            Serial.print(co2);
            Serial.print(" ppm, Temp: ");
            Serial.print(tempC, 2);
            Serial.print(" °C, Humidity: ");
            Serial.print(humRH, 2);
            Serial.println(" %RH");
        } else {
            Serial.println("Invalid frame received (End byte mismatch)");
        }
    }
}

