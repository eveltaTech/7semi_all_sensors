#include <SoftwareSerial.h>

// Define SoftwareSerial pins
SoftwareSerial mySerial(10, 11); // RX on pin 10, TX on pin 11

// Frame constants
const uint8_t START_BYTE = 0x7E;  // Start byte
const uint8_t END_BYTE = 0x7F;    // End byte
const uint8_t PAYLOAD_LENGTH = 16; // Payload length (4 floats: temperature, pressure, humidity, IAQ)
const uint8_t FRAME_LENGTH = 2 + PAYLOAD_LENGTH + 1; // Start byte + length + payload + end byte

// Buffer to store the frame
uint8_t frameBuffer[FRAME_LENGTH];
uint8_t receivedIndex = 0;
bool frameInProgress = false;

void setup() {
  Serial.begin(9600);   // Debugging serial port
  mySerial.begin(9600); // SoftwareSerial for STM32 communication
  Serial.println("Waiting for data...");
}

void loop() {
  while (mySerial.available()) {
    uint8_t byteReceived = mySerial.read();

    // Debugging: Print received bytes
   // Serial.print("Received Byte: 0x");
    //Serial.println(byteReceived, HEX);

    // Handle start of frame
    if (!frameInProgress && byteReceived == START_BYTE) {
      frameInProgress = true;
      receivedIndex = 0;
      frameBuffer[receivedIndex++] = byteReceived; // Store start byte
    } else if (frameInProgress) {
      // Store the received byte in the buffer
      frameBuffer[receivedIndex++] = byteReceived;

      // Check if the frame is complete
      if (receivedIndex == FRAME_LENGTH) {
        // Verify the end byte
        if (frameBuffer[FRAME_LENGTH - 1] == END_BYTE) {
          processFrame(frameBuffer); // Process the received frame
        } else {
          Serial.println("Invalid frame: missing END_BYTE");
        }
        frameInProgress = false; // Reset for the next frame
        receivedIndex = 0;
      }
    }
  }
}

// Function to process a complete frame
void processFrame(uint8_t *frame) {
  // Validate payload length
  if (frame[1] != PAYLOAD_LENGTH) {
    Serial.println("Invalid payload length");
    return;
  }

  // Extract the payload
  float temperature, pressure, humidity, iaqScore;
  memcpy(&temperature, &frame[2], sizeof(float)); // Temperature starts at index 2
  memcpy(&pressure, &frame[6], sizeof(float));    // Pressure starts at index 6
  memcpy(&humidity, &frame[10], sizeof(float));   // Humidity starts at index 10
  memcpy(&iaqScore, &frame[14], sizeof(float));   // IAQ score starts at index 14

  // Print the received data
  Serial.println("Received Frame:");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" Pa");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("IAQ Score: ");
  Serial.println(iaqScore);
  Serial.println("");
}
