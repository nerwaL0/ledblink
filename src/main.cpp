#include <Arduino.h>
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#include "config.h"
#include "ac_control.h"
#include "network_manager.h"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String userPath;
unsigned long lastFirebaseCheck = 0;
const int FIREBASE_CHECK_INTERVAL = 2000; // Cek setiap 2 detik

// Tambahkan variabel last untuk fan dan swing
int lastProtocol = -1, lastTemp = -1, lastFan = -1, lastMode = -1;
bool lastPower = false, lastSwing = false, isFirstRun = true;

String createPath(String base, String child) {
  String result = base;
  result += child;
  return result;
}

void setup() {
    Serial.begin(115200);
    setupAC();
    // Inisialisasi jaringan WiFi
    setupNetwork();
    userPath = "/";
    userPath += getDeviceID();
    userPath += "/settings";

    Serial.print("Path Firebase: ");
    Serial.println(userPath);
    // Firebase Config

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.signer.test_mode = true;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void loop() {
        // Cek tombol reset WiFi
        checkResetButton();
        // Cek interval untuk pengecekan Firebase
        if (millis()- lastFirebaseCheck >= 2000) { // Cek setiap 2 detik
         lastFirebaseCheck = millis(); // Update waktu terakhir cek

    if (Firebase.ready()) {
        int cProtocol = 0, cTemp = 24, cFan = 0, cMode = 0;
        bool cPower = false, cSwing = false;
        // 5. Gunakan userPath yang sudah kita buat di setup
        // Kita gunakan .c_str() agar library Firebase membacanya sebagai teks murni
        if (Firebase.getInt(fbdo, createPath(userPath, "/protocol_id").c_str())) cProtocol = fbdo.intData();
        if (Firebase.getBool(fbdo, createPath(userPath, "/power").c_str())) cPower = fbdo.boolData();
        if (Firebase.getInt(fbdo, createPath(userPath, "/temp").c_str())) cTemp = fbdo.intData();
        if (Firebase.getInt(fbdo, createPath(userPath, "/fan_speed").c_str())) cFan = fbdo.intData();
        if (Firebase.getBool(fbdo, createPath(userPath, "/swing").c_str())) cSwing = fbdo.boolData();
        if (Firebase.getInt(fbdo, createPath(userPath, "/mode").c_str())) cMode = fbdo.intData();
        // Cek jika ADA SALAH SATU yang berubah
        if (isFirstRun || cProtocol != lastProtocol || cPower != lastPower || 
            cTemp != lastTemp || cFan != lastFan || cSwing != lastSwing || cMode != lastMode) {
            
            // Kirim sinyal dengan parameter lengkap
            tembakSinyalAC(cProtocol, cPower, cTemp, cFan, cSwing, cMode);
            
            // Update status terakhir
            lastProtocol = cProtocol; 
            lastPower = cPower; 
            lastTemp = cTemp;
            lastFan = cFan; 
            lastSwing = cSwing;
            lastMode = cMode;
            isFirstRun = false;

        }
    } // if Firebase.ready()
  } // if interval check Firebase dan reset wifi
} 

