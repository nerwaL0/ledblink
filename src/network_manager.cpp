#include "network_manager.h"
#include <WiFiManager.h>

const int RESET_PIN = 0; 
unsigned long pressStartTime = 0;
bool isPressing = false;

void setupNetwork() {
    pinMode(RESET_PIN, INPUT_PULLUP);
    
    WiFiManager wm;
    // Autoconnect akan memblokir sampai terhubung atau timeout
    if(!wm.autoConnect("SmartRemote_Setup")) {
        Serial.println("Gagal terhubung, restarting...");
        ESP.restart();
    }
    Serial.println("Terhubung ke WiFi!");
}

void checkResetButton() {
    if (digitalRead(RESET_PIN) == LOW) {
        if (!isPressing) {
            pressStartTime = millis();
            isPressing = true;
            Serial.println("Tombol Boot ditekan, tahan 5 detik untuk reset...");
        } else if (millis() - pressStartTime >= 5000) {
            Serial.println("Mereset WiFi...");
            WiFiManager wm;
            wm.resetSettings();
            delay(1000);
            ESP.restart();
        }
    } else {
        if (isPressing) {
            Serial.println("Reset dibatalkan.");
            isPressing = false;
        }
    }
}

String getDeviceID() {
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    return mac;
}