#include "network_manager.h"
#include <WiFiManager.h>

const int RESET_PIN = 0; 
unsigned long pressStartTime = 0;
bool isPressing = false;

void setupNetwork() {
    pinMode(RESET_PIN, INPUT_PULLUP);
    
    WiFiManager wm;
    // menambahkan css custom jika diperlukan
    const char* custom_html = R"rawliteral(
<style>
    body {
        font-family: sans-serif;
        background-color: #f4f4f4;
        padding: 20px;
    }

    /* ===== ITEM WIFI (KARTU BESAR) ===== */
    a.q {
        display: block !important;
        padding: 30px 20px !important;
        margin: 16px 0 !important;
        background: #ffffff;
        border-radius: 18px;
        border: 2px solid #ccc;
        text-decoration: none !important;
    }

    /* ===== NAMA WIFI (SSID) ===== */
    .q .ssid {
        font-size: 2.6rem !important;   /* LEBIH BESAR DARI LABEL */
        font-weight: 700 !important;
        color: #000 !important;
        line-height: 1.3;
    }

    /* ===== SEMBUNYIKAN RSSI & ICON ===== */
    .q .r,
    .q .l {
        display: none !important;
    }

    /* ===== LABEL ===== */
    label {
        display: block;
        font-size: 2.2rem !important;
        font-weight: bold;
        margin-top: 40px;
        margin-bottom: 15px;
    }

    /* ===== INPUT FIELD ===== */
    input {
        height: 95px !important;
        font-size: 2.2rem !important;
        border-radius: 14px;
        width: 100%;
        border: 3px solid #999;
        padding-left: 18px;
    }

    /* ===== BUTTON ===== */
    button {
        background-color: #007bff !important;
        height: 120px !important;
        font-size: 2.6rem !important;
        font-weight: bold;
        border-radius: 22px;
        margin-top: 35px;
        width: 100%;
        color: #fff !important;
    }

    /* ===== FOKUS / AKSESIBILITAS ===== */
    a.q:focus,
    a.q:hover {
        outline: 4px solid #007bff;
    }
</style>

<script>
document.addEventListener('DOMContentLoaded', function () {
    document.querySelectorAll('label').forEach(function(label) {
        if (label.innerText.includes('SSID')) {
            label.innerText = 'Nama WiFi';
        }
    });
});
</script>
    <script>
        // Mengganti teks 'SSID' menjadi 'Nama WiFi' saat halaman dimuat
        document.addEventListener('DOMContentLoaded', function() {
            var labels = document.getElementsByTagName('label');
            for (var i = 0; i < labels.length; i++) {
                if (labels[i].innerHTML.includes('SSID')) {
                    labels[i].innerHTML = 'Nama WiFi';
                }
            }
        });
    </script>
    )rawliteral";
    wm.setCustomHeadElement(custom_html);
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