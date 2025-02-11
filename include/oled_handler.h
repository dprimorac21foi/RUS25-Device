#ifndef OLED_HANDLER_H
#define OLED_HANDLER_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_SDA      15
#define OLED_SCL      14

void setupOLED();
void displayMessage(const char* message);

#endif
