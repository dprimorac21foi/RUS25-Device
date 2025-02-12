#include <Arduino.h>
#include "oled.h"
#include "camera.h"
#include <WiFi.h>

//Wi-Fi podaci i funkcija za spajanja
const char* ssid = "HUAWEI_B535_1750";      // Replace with your WiFi name
const char* password = "DGA9Q9N5M0F";    // Replace with your WiFi password

void connectWifi() {
  Serial.println();
  Serial.print("Connecting to WiFi");
  
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup() { 

  Serial.begin(115200);

  // Setupa OLED zaslon, te ako je dobro setupan ispisuje poruku na OLED
  setupOLED();
  delay(3000);

  //Spajanje na Wi-Fi
  connectWifi();
  displayMessage("Povezano na Wi-Fi!");
  delay(3000);

  // Setupa kameru te ispisuje rezultat na OLED
  if (setupCamera())
    displayMessage("Kamera radi!");
  else
    displayMessage("Kamera ne radi!");
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost! Reconnecting...");
    connectWifi();
  }

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
