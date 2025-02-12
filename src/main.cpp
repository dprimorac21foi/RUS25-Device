#include <Arduino.h>
#include "oled.h"
#include "camera.h"


void setup() { 

  Serial.begin(115200);

  // Setupa OLED zaslon, te ako je dobro setupan ispisuje poruku na OLED
  setupOLED();
  delay(3000);

  // Setupa kameru te ispisuje rezultat na OLED
  if (setupCamera())
    displayMessage("Kamera radi!");
  else
    displayMessage("Kamera ne radi!");
}

void loop() {

  if (Serial.available() && Serial.read() == 't') {
    if (captureImage()) {
      displayMessage("Slika uhvacena!");
      delay(3000);
      freeCameraBuffer();
    }
    else  
      displayMessage("Slika nije uhvacena!");
  }
  delay(100);
}
