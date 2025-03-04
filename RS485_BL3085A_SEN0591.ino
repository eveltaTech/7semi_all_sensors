#define RS485_DE_RE 2  // Pin for controlling DE and /RE
#define BAUD_RATE 115200 // Set the baud rate (check SEN-0591 datasheet)

// Command to send to the SEN-0591
byte sensorCommand[] = {0x01, 0x03, 0x01, 0x01, 0x00, 0x01, 0xD4, 0x36};

void setup() {
  // Initialize serial communication
  Serial.begin(BAUD_RATE);       // UART to communicate with RS-485
  Serial.println("RS-485 Test");

  // Set DE and /RE as output and initialize in receive mode
  pinMode(RS485_DE_RE, OUTPUT);
  digitalWrite(RS485_DE_RE, LOW); // LOW = Receive mode
}

void loop() {
  // Transmit the command
  sendCommand(sensorCommand, sizeof(sensorCommand));

  // Wait briefly before switching to reception
  delay(2); // Adjust based on the timing required by the hardware setup

  // Switch to receive mode and wait for a response
  digitalWrite(RS485_DE_RE, LOW); // Ensure it is in receive mode
  delay(1); // Allow time for the response to arrive

  // Read and print the response
  readResponse();

  // Delay before the next communication cycle
  delay(2000);
}

void sendCommand(byte *command, size_t length) {
  // Switch to transmit mode
  digitalWrite(RS485_DE_RE, HIGH);

  // Send the command
  Serial.write(command, length);

  // Wait for the transmission to complete
  Serial.flush(); // Ensure all data has been sent before switching modes
}

void readResponse() {
  // Check if data is available
  if (Serial.available()) {
    Serial.print("Response: ");
    while (Serial.available()) {
      byte receivedByte = Serial.read();
      // Print each byte as hexadecimal
      Serial.print("0x");
      if (receivedByte < 16) Serial.print("0"); // Add leading zero for single-digit hex
      Serial.print(receivedByte, HEX);
      Serial.print(" ");
    }
    Serial.println(); // End the line after printing the full response
  } else {
    Serial.println("No response received");
  }
}