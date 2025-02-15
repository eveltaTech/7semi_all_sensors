// ESP32  Serial1 test - for loopback test connect pins 16 and 17
#define RXD1 12
#define TXD1 13

void setup() {
  // initialize both serial ports:
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);
  Serial.println();
  Serial.println("\n\nESP32 serial1 test Rx pin 16 Tx pin 17");
  Serial.write("for loopback test connect pin 16 to pin 17\n");
}

void loop() {
  // read from port 1, send to port 0:
  if (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
  }

  // read from port 0, send to port 1:
  if (Serial.available()) {
    int inByte = Serial.read();
    //Serial.write(inByte);
    Serial1.write(inByte);
  }
}