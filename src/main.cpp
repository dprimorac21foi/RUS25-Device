#include <Arduino.h>
#include "oled_handler.h"


void setup() {
    Serial.begin(115200);

    setupOLED();
    delay(4000);
    displayMessage("Hello, world!");

}

void loop() {
    
}
