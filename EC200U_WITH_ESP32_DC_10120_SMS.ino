#define RXD1 12
#define TXD1 13

void setup() {
  // Arduino communicates with SIM900 GSM shield at a baud rate of 19200
  // Make sure that corresponds to the baud rate of your module
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);
  // Give time to your GSM shield log on to network
  Serial.println();
  Serial.println("\n\nESP32 serial1 test Rx pin 16 Tx pin 17");
  Serial.write("for loopback test connect pin 16 to pin 17\n");
  delay(20000);  
 
  // Send the SMS
  sendSMS();
}

void loop() {
 
}
                                            
void sendSMS() {
  // AT command to set in SMS mode
  Serial1.print("AT+CMGF=1\r");
  Serial.println("AT+CMGF=1\r");
  delay(1000);

  // REPLACE THE X's WITH THE RECIPIENT'S MOBILE NUMBER
  // USE INTERNATIONAL FORMAT CODE FOR MOBILE NUMBERS
  Serial1.println("AT+CMGS=\"+91xxxxxxxxxx\"");
  Serial.println("AT+CMGS=\"+91xxxxxxxxxx\"");
  delay(1000);
 
  // REPLACE WITH YOUR OWN SMS MESSAGE CONTENT
  Serial1.println("Message example from ESP32");
  Serial.println("Message sent successfully");
  delay(1000);

  // End AT command with a ^Z, ASCII code 26
  Serial1.println((char)26);
  delay(500);
  Serial1.println();
  // Give module time to send SMS
  delay(5000);
}