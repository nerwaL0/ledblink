#include <Arduino.h>
#include "ac_control.h"
#include "network_manager.h"
#include "firebase_manager.h"

void setup() {
    Serial.begin(115200);
    setupAC();
    setupNetwork();   // Inisialisasi WiFi Manager
    setupFirebase();  // Inisialisasi Firebase & Path
}

void loop() {
    checkResetButton();      // Cek tombol reset WiFi
    handleFirebaseUpdates(); // Cek dan eksekusi perintah dari Firebase
}