#ifndef ESP32CAM_H
#define ESP32CAM_H

#include <Arduino.h>
#include "esp_camera.h"

bool setupCamera();
camera_fb_t* captureImage();
void releaseImage(camera_fb_t *fb);

#endif
