#include "network_manager.h"
#include <WiFiManager.h>
#include <Ticker.h>

Ticker ticker;
const int LED_PIN = 2; // LED internal untuk indikasi status WiFi
const int RESET_PIN = 0; 
unsigned long pressStartTime = 0;
bool isPressing = false;

void tick() {
    int state = digitalRead(LED_PIN);
    digitalWrite(LED_PIN, !state); // Toggle LED
}
void setupNetwork() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(RESET_PIN, INPUT_PULLUP);
    ticker.attach(0.5, tick); // LED berkedip setiap 500ms untuk indikasi WiFi
    
    WiFiManager wm;
    // menambahkan css custom jika diperlukan
    const char* custom_html = R"rawliteral(
<style>

/* ===== GLOBAL SCALE ===== */
body {
    font-family: sans-serif;
    background-color: #f4f4f4;
    padding: 25px;
    font-size: 26px !important;
}

/* ===== WIFI SSID LIST ===== */
a[data-ssid] {
    display: block !important;
    font-size: 44px !important;
    font-weight: 900 !important;
    padding: 55px 30px !important;
    margin: 30px 0 !important;
    line-height: 1.4 !important;
    border-radius: 22px;
    background: #ffffff;
    border: 3px solid #ccc;
}

/* Hide signal bars */
div.q {
    display: none !important;
}

/* ===== FORM LABELS ===== */
label {
    display: block;
    font-size: 34px !important;
    font-weight: 800 !important;
    margin-top: 50px;
    margin-bottom: 20px;
}

/* ===== INPUT FIELDS ===== */
input[type="text"],
input[type="password"] {
    height: 120px !important;
    font-size: 34px !important;
    border-radius: 18px;
    width: 100%;
    border: 3px solid #999;
    padding-left: 25px;
}

/* ===== BUTTONS ===== */
button,
input[type="submit"] {
    background-color: #007bff !important;
    height: 140px !important;
    font-size: 36px !important;
    font-weight: 900 !important;
    border-radius: 26px !important;
    margin-top: 45px;
    width: 100%;
    color: #fff !important;
}

/* ===== CHECKBOX (Show Password) ===== */
input[type="checkbox"] {
    transform: scale(2.2);
    margin-right: 15px;
}

/* ===== GENERAL TEXT (messages like "No AP set") ===== */
.msg,
div,
p,
span {
    font-size: 30px !important;
}

/* ===== TOUCH FRIENDLY SPACING ===== */
br {
    display: block;
    margin-bottom: 20px;
}

</style>

<script>
document.addEventListener('DOMContentLoaded', function () {

    /* Rename SSID label to Nama WiFi */
    document.querySelectorAll('label').forEach(function(label) {
        if (label.innerText.includes('SSID')) {
            label.innerText = 'Nama WiFi';
        }
    });

});
</script>
)rawliteral";

    wm.setCustomHeadElement(custom_html);
    // Autoconnect akan memblokir sampai terhubung atau timeout
    if(!wm.autoConnect("SmartRemote_Setup")) {
        Serial.println("Gagal terhubung, restarting...");
        ESP.restart();
    }
    ticker.detach(); // Berhenti berkedip setelah terhubung
    digitalWrite(LED_PIN, HIGH); // LED menyala solid untuk indikasi sukses
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