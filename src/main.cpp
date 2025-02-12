#include <Arduino.h>
#include "oled.h"
#include "camera.h"
#include <WiFi.h>
#include <time.h>
#include "base64.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "AzIoTSasToken.h"
#include "SerialLogger.h"
#include <az_core.h>
#include <az_iot.h>

// Wi-Fi Credentials
const char* ssid = "HUAWEI_B535_1750";  
const char* password = "DGA9Q9N5M0F";   

// IoT Hub Credentials
const char* iotHubHost = "iot-hub-rus-dprimorac.azure-devices.net";  
const char* deviceId = "esp32cam-dprimorac";  
const char* deviceKey = "cCz4jKtXkDn+GkJQ/Rf5U7WhDPEps6TP+KZwLYL+r5g="; 

// MQTT Authentication Buffers
char mqttClientId[64], mqttUsername[128], mqttPasswordBuffer[256], sasSignatureBuffer[256];

// Networking
WiFiClientSecure wifiClient;
PubSubClient mqtt(wifiClient);

// SAS Token Object
az_iot_hub_client client;
AzIoTSasToken sasToken(
  &client,
  az_span_create_from_str((char*)deviceKey),
  AZ_SPAN_FROM_BUFFER(sasSignatureBuffer),
  AZ_SPAN_FROM_BUFFER(mqttPasswordBuffer)
);

// Function to Initialize Time
void initializeTime() {
    Logger.Info("Setting time using SNTP...");
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    time_t now = time(nullptr);
    while (now < 1704067200) { // Wait until time is valid
        delay(500);
        now = time(nullptr);
    }

    Logger.Info("Time successfully set!");
}

// Function to Connect to WiFi
void connectWifi() {
    WiFi.begin(ssid, password);
    Logger.Info("Connecting to WiFi...");

    int timeoutCounter = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if (++timeoutCounter > 20) {
            Logger.Error("Wi-Fi connection failed, restarting...");
            ESP.restart();
        }
    }

    Logger.Info("WiFi connected, IP: " + WiFi.localIP().toString());
}

// Function to Connect to MQTT
bool connectMQTT() {
    Logger.Info("Generating SAS token...");
    if (sasToken.Generate(3600) != 0) { 
        Logger.Error("Failed to generate SAS token!");
        return false;
    }

    snprintf(mqttUsername, sizeof(mqttUsername), "%s/%s/?api-version=2021-04-12",
             iotHubHost, deviceId);
    snprintf(mqttClientId, sizeof(mqttClientId), "%s", deviceId);
    
    Logger.Info("Connecting to Azure IoT Hub via MQTT...");
    mqtt.setServer(iotHubHost, 8883);

    while (!mqtt.connected()) {
        if (mqtt.connect(mqttClientId, mqttUsername, (const char*)az_span_ptr(sasToken.Get()))) {
            Logger.Info("Connected to Azure IoT Hub!");
            mqtt.subscribe(AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC);
            return true;
        } else {
            Logger.Error("MQTT connection failed, retrying...");
            delay(5000);
        }
    }
    return false;
}

// Function to Send Image Data to Azure IoT Hub
void sendPicture() {
  camera_fb_t* slika = captureImage();
  String base64Image = base64::encode(slika->buf, slika->len);
  freeCameraBuffer(slika);

  DynamicJsonDocument doc(8192);
  doc["deviceId"] = deviceId;
  doc["image"] = base64Image;
  doc["timestamp"] = millis();

  String jsonString;
  serializeJson(doc, jsonString);

  Logger.Info("Sending image data to Azure...");
  mqtt.publish(("devices/" + String(deviceId) + "/messages/events/").c_str(), jsonString.c_str());
}

// Function to Handle Cloud-to-Device Messages
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';
    String message = String((char*)payload);

    Logger.Info("Message received: " + message);
    
    if (message == "capture") {
        sendPicture();
    }
}

// Setup Function
void setup() {
    Serial.begin(115200);
    setupOLED();
    delay(3000);

    connectWifi();
    displayMessage("Connected to WiFi!");
    delay(3000);

    initializeTime();

    if (setupCamera()) {
        displayMessage("Camera working!");
    } else {
        displayMessage("Camera not working!");
    }

    mqtt.setCallback(mqttCallback);
    connectMQTT();
}

// Loop Function
void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Logger.Error("WiFi lost! Reconnecting...");
        connectWifi();
    }

    if (!mqtt.connected()) {
        Logger.Error("MQTT lost! Reconnecting...");
        connectMQTT();
    }

    mqtt.loop();

    if (Serial.available() && Serial.read() == 't') {
        sendPicture();
    }
    delay(100);
}
