#include <SoftwareSerial.h>

// Use SoftwareSerial on pins 10 (RX), 11 (TX)
SoftwareSerial modbus(10, 11);  // RX, TX
#define MAX485_DE_RE 2

// Modbus RTU request: Slave 0x00, Function 0x03, Read 2 registers from 0x0001
byte request[] = {0x00, 0x03, 0x00, 0x01, 0x00, 0x02, 0x94, 0x1A};

void setup() {
  Serial.begin(115200);         // Serial monitor output
  modbus.begin(9600);           // RS485 communication
  pinMode(MAX485_DE_RE, OUTPUT);
  digitalWrite(MAX485_DE_RE, 0);  // Start in receive mode

  Serial.println("Starting RS485 Modbus Sensor Reader...");
}

void loop() {
  // === Transmit Modbus request ===
  digitalWrite(MAX485_DE_RE, 1);
  delay(2);  // Settle time

  modbus.write(request, sizeof(request));
  modbus.flush();  // Wait until done sending
  digitalWrite(MAX485_DE_RE, 0);

  delay(50);  // Wait for response

  // === Receive response ===
  byte response[9];
  int i = 0;
  unsigned long start = millis();
  while (millis() - start < 200 && i < 9) {
    if (modbus.available()) {
      response[i++] = modbus.read();
    }
  }

  // === Validate and decode ===
  if (i == 9 && response[0] == 0x00 && response[1] == 0x03 && response[2] == 0x04) {
    uint16_t rawTemp = (response[3] << 8) | response[4];
    uint16_t rawHum  = (response[5] << 8) | response[6];

    float temperature = rawTemp / 10.0;
    float humidity = rawHum / 10.0;

    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print(" °C\tHumidity: ");
    Serial.print(humidity, 1);
    Serial.println(" %RH");
  } else {
    Serial.println("No/Invalid response.");
  }

  delay(2000);  // Wait before next cycle
}
