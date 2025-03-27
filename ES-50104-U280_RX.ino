#include <SoftwareSerial.h>

// Constants for frame validation
#define FRAME_START_BYTE 0xAA
#define FRAME_END_BYTE 0x55
#define FRAME_SIZE 14

// Define SoftwareSerial RX and TX pins
#define RX_PIN 10
#define TX_PIN 11

// Create a SoftwareSerial object
SoftwareSerial softSerial(RX_PIN, TX_PIN);

// Variables to hold received data
float receivedTemperature = 0.0;
float receivedPressure = 0.0;
float receivedHumidity = 0.0;

void setup() {
  // Initialize SoftwareSerial
  softSerial.begin(9600);

  // Initialize Serial Monitor
  Serial.begin(9600);
  Serial.println("Starting BME280 Frame Receiver with SoftwareSerial...");
}

void loop() {
  // Check if enough data is available in the software serial buffer
  if (softSerial.available() >= FRAME_SIZE) {
    uint8_t frame[FRAME_SIZE];
    for (int i = 0; i < FRAME_SIZE; i++) {
      frame[i] = softSerial.read(); // Read frame data byte by byte
    }

    // Debug: Print the raw frame
    //Serial.println("Received Raw Frame:");
    for (int i = 0; i < FRAME_SIZE; i++) {
      // Serial.print("0x");
      // Serial.print(frame[i], HEX);
      // Serial.print(" ");
    }
    Serial.println();

    // Validate the start and end bytes
    if (frame[0] == FRAME_START_BYTE && frame[13] == FRAME_END_BYTE) {
      // Extract data using memcpy
      memcpy(&receivedTemperature, &frame[1], 4);
      memcpy(&receivedPressure, &frame[5], 4);
      memcpy(&receivedHumidity, &frame[9], 4);

      // Debug: Print the received data
      Serial.print("Temperature: ");
      Serial.print(receivedTemperature);
      Serial.println(" °C");

      Serial.print("Pressure: ");
      Serial.print(receivedPressure);
      Serial.println(" hPa");

      Serial.print("Humidity: ");
      Serial.print(receivedHumidity);
      Serial.println(" %RH");
      Serial.println(" ");
      Serial.println(" ");
    } else {
      // Invalid frame received
      Serial.println("Error: Invalid frame received.");
    }
  } else {
    // No data available
    Serial.println("Waiting for data...");
    delay(1000); // Wait for 1 second before checking again
  }
}
