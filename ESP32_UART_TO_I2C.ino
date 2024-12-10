#include <HardwareSerial.h>

// Create a hardware serial object
HardwareSerial mySerial(1); // UART1 (can use UART2 if desired)

// Pin definitions for UART communication (adjust as needed)
#define RXD2 16 // GPIO16 for RX
#define TXD2 17 // GPIO17 for TX

void setup() {
  Serial.begin(115200);      // Debugging via Serial Monitor
  mySerial.begin(9600, SERIAL_8N1, RXD2, TXD2); // Baud rate and pins for SC18IM704 communication
  delay(1000);               // Allow time for initialization

  // Declare static arrays for each command
  uint8_t command1[] = {0x56, 0x50};        // Command 1: 56 50
  uint8_t command2[] = {0x52, 0x00, 0x50};  // Command 2: 52 00 50
  uint8_t command3[] = {0x52, 0x01, 0x50};  // Command 3: 52 01 50

  // Send and handle response for each command
  sendAndPrintResponse(command1, sizeof(command1));
  sendAndPrintResponse(command2, sizeof(command2));
  sendAndPrintResponse(command3, sizeof(command3));
}

void loop() {
  // No repeated actions in loop
}

// Function to send a command and print the response
void sendAndPrintResponse(uint8_t* cmd, size_t len) {
  // Clear the response buffer before sending the command
  while (mySerial.available()) {
    mySerial.read(); // Clear any lingering bytes in the buffer
  }

  // Send the command
  Serial.print("Sending: ");
  for (size_t i = 0; i < len; i++) {
    mySerial.write(cmd[i]);   // Send command byte by byte
    Serial.print(cmd[i], HEX); // Print sent byte in HEX
    Serial.print(" ");
  }
  Serial.println();

  delay(1000); // Wait for the response

  // Read and print the response in HEX only
  if (mySerial.available()) {
    Serial.print("Response (HEX): ");
    while (mySerial.available()) {
      uint8_t response = mySerial.read(); // Read a byte
      Serial.print(response, HEX);        // Print response in HEX format
      Serial.print(" ");
    }
    Serial.println();
  } else {
    Serial.println("No response received.");
  }

  delay(1000); // Small delay before sending the next command
}
