#include <SoftwareSerial.h>

const int frameSize = 13;
uint8_t buffer[frameSize];

// Define software serial on pins 10 (RX), 11 (TX)
SoftwareSerial mySerial(10, 11); // RX, TX

void setup() {
    mySerial.begin(9600); // Initialize Software Serial Communication
    Serial.begin(9600);   // Optional: for debugging to Serial Monitor
}

void loop() {
    if (mySerial.available() >= frameSize) 
    {
        mySerial.readBytes(buffer, frameSize);
        
        // Validate start byte
        if (buffer[0] != 0xAA) 
        {
            Serial.println("Invalid frame start");
            return;
        }
        
        // Extract values
        int32_t temp_int, hum_int, voc_index;
        memcpy(&temp_int, &buffer[1], sizeof(int32_t));
        memcpy(&hum_int, &buffer[5], sizeof(int32_t));
        memcpy(&voc_index, &buffer[9], sizeof(int32_t));
        
        // Convert to float
        float temperature = *(float*)&temp_int;
        float humidity = *(float*)&hum_int;
        uint32_t voc = *(uint32_t*)&voc_index;
        
        // Ensure VOC index is positive
        if (voc_index < 0) voc_index = 0;
        
        // Print decoded values
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" C, Humidity: ");
        Serial.print(humidity);
        Serial.print(" %, VOC Index: ");
        Serial.println(voc);
    }
}

