#include <Arduino.h>
#include "oled.h"
#include "camera.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Wi-Fi podaci 
const char* ssid = "HUAWEI_B535_1750";  
const char* password = "DGA9Q9N5M0F";   

// IoT Hub podaci
char* iotHubHost = "iot-hub-rus-dprimorac.azure-devices.net";  
char* deviceId = "esp32cam-dprimorac";  
char* deviceKey = "cCz4jKtXkDn+GkJQ/Rf5U7WhDPEps6TP+KZwLYL+r5g="; 

// MQTT i SaS
char mqttClientId[64];
char mqttUsername[128];
char mqttPasswordBuffer[200];
uint8_t sasSignatureBuffer[256];

// Slanje slike na IoT Hub
void createJson(const char* base64Image, const char* timestamp){
  DynamicJsonDocument doc(8192);

  doc["link"] = "http://yourserver.com/image_upload"; 
  doc["image"] = base64Image; 
  doc["timestamp"] = timestamp;  

  String jsonString;
  serializeJson(doc, jsonString);
  Serial.println(jsonString);
}

/* Spajanje na MQTT i generiranje tokena
void connectMQTT() {
  Serial.println("Generiranje SAS tokena..");

  if (sasToken.Generate(3600) != 0) { 
      Serial.println("Failed to generate SAS token!");
      return;
  }
  String sasTokenStr = String(reinterpret_cast<const char*>(az_span_ptr(sasToken.Get())));

  snprintf(mqttUsername, sizeof(mqttUsername), "%s/%s/?api-version=2021-04-12",
           iotHubHost, deviceId);

  snprintf(mqttClientId, sizeof(mqttClientId), "%s", deviceId);
  
  Serial.println("Connecting to Azure IoT Hub via MQTT...");
  mqtt.setServer(iotHubHost, 8883);
  while (!mqtt.connected()) {
      Serial.print("Attempting MQTT connection... ");
      if (mqtt.connect(mqttClientId, mqttUsername, sasTokenStr.c_str())) {
          Serial.println("Connected to Azure IoT Hub!");
      } else {
          Serial.print("Failed, state: ");
          Serial.println(mqtt.state());
          Serial.println("Retrying in 5 seconds...");
          delay(5000);
      }
  }
}
*/

// Spajanje uređaja na Wi-Fi
void connectWifi() {
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int timeoutCounter=0;
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");

      if (timeoutCounter > 20){
        Serial.println("Neuspjelo spajanje na Wi-Fi, ponovno pokretanje...");
        ESP.restart();
      }
  }
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
