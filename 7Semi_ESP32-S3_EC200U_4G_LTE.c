// Define the hardware serial connection to the EC200U module
#define EC200U_RX 12 // Define the pin for RX (EC200U TX)
#define EC200U_TX 13 // Define the pin for TX (EC200U RX)

HardwareSerial ec200u(1); // Using UART1 for EC200U communication

String phoneNumber = "+91XXXXXXXXXX"; // Replace with the recipient's phone number

void setup() {
  // Initialize serial communications
  Serial.begin(115200); // For serial monitor output
  ec200u.begin(115200, SERIAL_8N1, EC200U_RX, EC200U_TX); // Initialize UART1 with the correct RX/TX pins
  
  // Wait for the EC200U module to initialize
  delay(3000);
  
  // Test the communication with the EC200U module
  if (!sendATCommand("AT", 1000)) {
    Serial.println("Error: Communication with EC200U module failed.");
    return;
  }

  // Set SMS text mode
  if (!sendATCommand("AT+CMGF=1", 1000)) {
    Serial.println("Error: Failed to set SMS text mode.");
    return;
  }

  // Enable GPS
  if (!sendATCommand("AT+QGPS=1", 2000)) {
    Serial.println("Error: Failed to enable GPS.");
    return;
  }
  
  Serial.println("EC200U module initialized successfully.");
}

void loop() {
  // Get the GPS data
  String gpsData = sendATCommand("AT+QGPSLOC=2", 10000); // Increased timeout for GPS data retrieval
  Serial.println("GPS Data: " + gpsData);

  if (gpsData.indexOf("+QGPSLOC:") != -1) {
    // Parse the GPS data
    String latitude = parseLatitude(gpsData);
    String longitude = parseLongitude(gpsData);

    // Format the GPS data into a Google Maps URL
    String url = "https://maps.google.com/?q=" + latitude + "," + longitude;

    // Send the SMS
    if (sendSMS(phoneNumber, url)) {
      Serial.println("SMS sent successfully.");
    } else {
      Serial.println("Error: Failed to send SMS.");
    }

    // Wait for some time before sending the next SMS
    delay(60000); // Wait for 1 minute
  } else {
    Serial.println("Failed to retrieve GPS data.");
    delay(10000); // Wait for 10 seconds before retrying
  }
}

String sendATCommand(String command, int timeout) {
  ec200u.println(command);
  delay(100); // Small delay for command execution
  String response = "";
  long startTime = millis();

  while (millis() - startTime < timeout) {
    while (ec200u.available()) {
      response += ec200u.readString();
    }

    if (response.length() > 0) {
      break; // Exit loop if response is received
    }
  }

  Serial.println("AT Command: " + command);
  Serial.println("AT Command Response: " + response);
  
  return response;
}

String parseLatitude(String gpsData) {
  int startIndex = gpsData.indexOf(",") + 1;
  int endIndex = gpsData.indexOf(",", startIndex);
  String latitude = gpsData.substring(startIndex, endIndex);
  latitude.trim();  // Ensure no extra spaces
  return latitude;
}

String parseLongitude(String gpsData) {
  // Find the start index of the longitude value
  int startIndex = gpsData.indexOf(",", gpsData.indexOf(",") + 1) + 1; // Skip the first two commas
  
  // Find the end index of the longitude value
  int endIndex = gpsData.indexOf(",", startIndex); // Find the next comma after the start index
  
  // Extract the longitude substring
  String longitude = gpsData.substring(startIndex, endIndex);
  longitude.trim();  // Trim leading and trailing spaces, if any
  
  return longitude;
}

bool sendSMS(String phoneNumber, String message) {
  // Start the SMS
  ec200u.println("AT+CMGS=\"" + phoneNumber + "\"");
  delay(1000);

  // Send the SMS message
  ec200u.println(message);
  delay(1000);

  // End the SMS with Ctrl+Z (ASCII 26)
  ec200u.write(26); 
  delay(1000);

  // Check for "OK" response
  String response = sendATCommand("", 1000);
  return response.indexOf("OK") != -1;
}
