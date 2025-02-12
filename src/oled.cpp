#include "oled.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setupOLED() {
    Wire.begin(OLED_SDA, OLED_SCL);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 alokacija nije uspjela!");
        while (1);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println("OLED setupan!");
    display.display();
}

void displayMessage(const char* message) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println(message);
    display.display();
}

