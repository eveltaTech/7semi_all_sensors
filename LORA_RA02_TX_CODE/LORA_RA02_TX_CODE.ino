#include <SPI.h>
#include <LoRa.h>

#define BUTTON_PIN 21  
#define SS 8
#define RST 4
#define DIO0 16

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    SPI.begin(5, 6, 7, 8);
    LoRa.setPins(SS, RST, DIO0);

    if (!LoRa.begin(433E6)) {
        Serial.println("LoRa initialization failed!");
        while (1);
    }
    Serial.println("LoRa Transmitter Initialized.");
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        Serial.println("Button Pressed! Sending LED_ON...");
        LoRa.beginPacket();
        LoRa.print("LED_ON");
        LoRa.endPacket();
        delay(50);
    }
    else{
        Serial.println("Button released! Sending LED_OFF...");
        LoRa.beginPacket();
        LoRa.print("LED_OFF");
        LoRa.endPacket();
        delay(50);
    }
}