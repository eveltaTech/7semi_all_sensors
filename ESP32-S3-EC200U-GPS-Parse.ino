#define RXD1 12  // Connect this to the module's TX pin
#define TXD1 13  // Connect this to the module's RX pin

// Interval (in milliseconds) between GPS location requests:
static unsigned long lastGPSTime = 0;
static const unsigned long GPS_REQUEST_INTERVAL = 10000;

HardwareSerial modemSerial(1); // For clarity, rename Serial1 to modemSerial


/**
 * Helper: Extract the n-th comma-separated field from 'data'
 * Returns empty string if out of range.
 */
String getValue(const String &data, char separator, int index) {
  int found = 0;
  int start = 0;
  int end   = -1;

  for (int i = 0; i < data.length(); i++) {
    if (data.charAt(i) == separator || i == data.length() - 1) {
      found++;
      if (found == index + 1) {
        end = (i == data.length() - 1) ? (i + 1) : i;
        return data.substring(start, end);
      }
      start = i + 1;
    }
  }
  return "";
}

/**
 * Helper: Parse the response from "AT+QGPSLOC=2" 
 *         and extract latitude & longitude.
 * The typical response line might look like:
 *   +QGPSLOC: 091152.000,19.05465,73.01633,1.8,-68.5,3,081.90,1.3,0.7,240225,17
 * or it might also include trailing "OK", so we trim it out.
 *
 * Returns the latitude/longitude in a human-readable format
 * or returns an empty string if invalid data.
 */
String parseGPSResponse(const String &response) {
  // 1) Find the line that contains "+QGPSLOC:"
  int startIndex = response.indexOf("+QGPSLOC:");
  if (startIndex == -1) {
    // Not found
    return "";
  }

  // 2) Extract everything after "+QGPSLOC:"
  //    We skip 9 characters if we want to skip "+QGPSLOC"
  //    Actually, the substring we need starts after the colon
  startIndex = response.indexOf(':', startIndex);
  if (startIndex == -1) {
    return "";
  }

  // Substring from the colon onward
  // e.g. ": 091152.000,19.05465,73.01633,1.8,-68.5,3,081.90,1.3,0.7,240225,17"
  String dataPart = response.substring(startIndex + 1); 
  dataPart.trim(); // remove whitespace/newlines

  // The format typically is:
  //   time,latitude,longitude,<... other fields ...>
  //   indexes: 
  //    - field[0] -> time (e.g., 091152.000)
  //    - field[1] -> latitude  (e.g., 19.05465)
  //    - field[2] -> longitude (e.g., 73.01633)
  
  String latitude  = getValue(dataPart, ',', 1);
  String longitude = getValue(dataPart, ',', 2);

  if (latitude.length() == 0 || longitude.length() == 0) {
    // Parsing failed or invalid data
    return "";
  }

  // Build a human-readable string
  String result = "Latitude: " + latitude + ", Longitude: " + longitude;
  return result;
}


void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);

  // Initialize modemSerial (UART1) on pins RXD1 and TXD1 at 115200 baud
  modemSerial.begin(115200, SERIAL_8N1, RXD1, TXD1);

  // Wait a bit for the module to power up and be ready
  Serial.println("\nStarting EC200U AT command test...");
  delay(5000);

  // 1) Basic check
  sendATCommand("AT", 2000);

  // 2) Get module info
  sendATCommand("ATI", 2000);

  // 3) Check signal strength
  sendATCommand("AT+CSQ", 2000);

  // 4) Turn Off the GPS (in case it was previously on)
  sendATCommand("AT+QGPSEND", 2000);
   
  // 5) Enable GPS with specific parameters
  //    Example: "AT+QGPS=1,1,0,0,1"
  //    Adjust parameters as needed per module's documentation
  sendATCommand("AT+QGPS=1,1,0,0,1", 2000);

  // Give the module time to acquire satellites (40 seconds often recommended)
  Serial.println("Waiting 40 seconds for GPS lock...");
  delay(40000);
}


void loop() {
  // We want to request GPS data every 10 seconds
  unsigned long now = millis();

  if (now >= lastGPSTime) {
    // Schedule the next trigger EXACTLY 10 seconds after the previous
    lastGPSTime += GPS_REQUEST_INTERVAL;

    // 6) Request GPS location
    String response = sendATCommand("AT+QGPSLOC=2", 2000);

    // 7) Parse out latitude and longitude
    String locationStr = parseGPSResponse(response);
    if (locationStr.length() > 0) {
      Serial.println(locationStr); 
    } else {
      Serial.println("Failed to parse valid latitude/longitude.");
    }
  }
}

String sendATCommand(const String &command, unsigned long timeout) {
  String response;

  // Clear any stale data in the buffer
  while (modemSerial.available()) {
    modemSerial.read();
  }

  // Send the command
  modemSerial.println(command);
  Serial.print("\n> ");
  Serial.println(command);

  // Read the response until timeout
  unsigned long startTime = millis();
  while (millis() - startTime < timeout) {
    while (modemSerial.available()) {
      char c = (char)modemSerial.read();
      response += c;
    }
  }

  // Print the response to Serial Monitor
  Serial.print("< Response: ");
  Serial.println(response);

  // Return the response (if further parsing is needed)
  return response;
}
