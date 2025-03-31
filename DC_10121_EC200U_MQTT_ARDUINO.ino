#include <SoftwareSerial.h>

#define SerialMon Serial
SoftwareSerial SerialAT(2, 3);  // RX, TX

#define MODEM_BAUDRATE 115200  // Use 115200 or match with EC200U-CN setting

const char apn[]  = "airtelgprs.com";
const char user[] = "";
const char pass[] = "";

const char mqttServer[]   = "io.adafruit.com";
const int  mqttPort       = 1883;
const char mqttUser[]     = "KOmkar";
const char mqttPassword[] = "";
const char mqttPublishTopic[] = "";

unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 15000;

void setup() {
  SerialMon.begin(9600);
  SerialAT.begin(MODEM_BAUDRATE);

  delay(2000);
  SerialMon.println("Initializing modem...");

  modem_init();

  SerialMon.println("Disconnecting previous MQTT sessions...");
  sendATCommand("AT+QMTDISC=0");
  sendATCommand("AT+QMTOPEN=0,\"" + String(mqttServer) + "\"," + String(mqttPort));
  checkResponse("AT+QMTCONN=0,\"123\",\"" + String(mqttUser) + "\",\"" + mqttPassword + "\"");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastPublishTime >= publishInterval) {
    publishData();
    lastPublishTime = currentMillis;
  }
}

void publishData() {
  static int counter = 0;
  String payload = String(counter++);

  String csqValue = getCSQValue();
  SerialMon.println("CSQ Value: " + csqValue);

  SerialMon.println("Publishing data: " + payload);
  sendATCommand("AT+QMTPUB=0,0,0,0,\"" + String(mqttPublishTopic) + "\"");

  delay(200);  // Wait before sending payload
  SerialAT.print(payload);
  SerialAT.write(0x1A);  // Ctrl+Z to send
  delay(1000);
}

void modem_init() {
  sendATCommand("AT");
  sendATCommand("ATE0");
  sendATCommand("AT+CPIN?");
  sendATCommand("AT+CSQ");
  sendATCommand("AT+CREG?");
  sendATCommand("AT+CGATT?");
}

void sendATCommand(const String& command) {
  SerialMon.print("Sending: ");
  SerialMon.println(command);
  SerialAT.println(command);
  delay(1000);

  unsigned long timeout = millis();
  while (millis() - timeout < 3000) {
    while (SerialAT.available()) {
      SerialMon.write(SerialAT.read());
    }
  }
}

void checkResponse(const String& command) {
  SerialMon.print("Sending: ");
  SerialMon.println(command);
  SerialAT.println(command);
  delay(1000);

  String response = "";
  unsigned long timeout = millis();
  while (millis() - timeout < 3000) {
    while (SerialAT.available()) {
      char c = SerialAT.read();
      response += c;
      SerialMon.print(c);
    }
  }

  if (response.indexOf("ERROR") != -1) {
    SerialMon.println("Error in response!");
    while (true);  // Halt on error
  }
}

String getCSQValue() {
  while (SerialAT.available()) SerialAT.read();

  SerialAT.println("AT+CSQ");
  delay(500);

  String response = "";
  unsigned long timeout = millis();
  while (millis() - timeout < 1000) {
    while (SerialAT.available()) {
      response += SerialAT.readString();
    }
  }

  SerialMon.println("Response (CSQ): " + response);

  int index = response.indexOf("+CSQ: ");
  if (index != -1) {
    String csqPart = response.substring(index + 6);
    int commaIndex = csqPart.indexOf(',');
    if (commaIndex != -1) {
      String rssiValue = csqPart.substring(0, commaIndex);
      rssiValue.trim();
      return rssiValue;
    }
  }
  return "N/A";
}
