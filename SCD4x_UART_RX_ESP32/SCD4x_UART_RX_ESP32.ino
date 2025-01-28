// Define the UART pins (RX, TX) for ESP32
#define RX_PIN 16 // GPIO16 as RX
#define TX_PIN 17 // GPIO17 as TX

// Frame configuration
#define START_BYTE 0xAA
#define END_BYTE 0x55
#define FRAME_SIZE 8

void setup() {
    // Initialize Serial1 with defined RX and TX pins
    Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Baud rate 9600

    // Initialize hardware serial for debugging
    Serial.begin(115200); // Higher baud rate for debugging
    Serial.println("ESP32 Serial Decoder Started");
}

void loop() {
    uint8_t frame[FRAME_SIZE];

    // Wait for start byte
    if (Serial1.available() > 0 && Serial1.read() == START_BYTE) {
        // Read the rest of the frame
        for (int i = 1; i < FRAME_SIZE; i++) {
            while (!Serial1.available()); // Wait for data
            frame[i] = Serial1.read();
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
