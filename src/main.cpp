#include <Arduino.h>
#include "oled.h"
#include "esp32cam.h"


void setup() {
    Serial.begin(115200);

    if (setupCamera()) {
      displayMessage("Camera Initialized +");
    } else {
      Serial.println("Camera failed to initialize. Restarting in 5 seconds...");
      delay(5000);
      ESP.restart();
    } 

    setupOLED();
    delay(4000);
    displayMessage("Setting up camera...");
    delay(2000);

    delay(2000);

    
}

void loop() {
    
}
