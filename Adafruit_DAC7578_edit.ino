#include <Adafruit_DACX578.h>

Adafruit_DACX578 dac(12); // 12-bit resolution

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("Adafruit DACX578 Set All Channels Test");

  if (!dac.begin(0x4C)) {
    Serial.println("Failed to find DAC7578 chip");
    while (1)
      delay(10);
  }

  Serial.println("DAC7578 initialized");

  // Set I2C frequency to 100 kHz 
  Wire.setClock(100000);

  // Set all 8 channels to a fixed value (example: mid-scale 2048)
  uint16_t value = 2048;
  for (uint8_t channel = 0; channel < 8; channel++) {
    dac.writeAndUpdateChannelValue(channel, value);
    delay(200);
  }

  Serial.println("All DAC channels set to value: " + String(value));
}

void loop() {
  // Empty since no continuous updates are needed
}
