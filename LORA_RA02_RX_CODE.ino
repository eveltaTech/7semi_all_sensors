#include <SPI.h>
#include <LoRa.h>

#define LED_PIN 13     
#define SS 5       
#define RST 4       
#define DIO0 26     

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    
    SPI.begin(18, 19, 23, 5);  
    LoRa.setPins(SS, RST, DIO0);

    if (!LoRa.begin(433E6)) {  
        Serial.println("LoRa initialization failed!");
        while (1);
    }
    Serial.println("LoRa Receiver Initialized.");
}

void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        Serial.print("Received packet: ");
        
        String receivedData = "";
        while (LoRa.available()) {
            receivedData += (char)LoRa.read();
        }

        Serial.println(receivedData);

    
        if (receivedData == "LED_ON") {
            digitalWrite(LED_PIN, HIGH);
            Serial.println("LED Turned ON!");
        } else if (receivedData == "LED_OFF") {
            digitalWrite(LED_PIN, LOW);
            Serial.println("LED Turned OFF!");
        }
    }
}