#include <Arduino.h>
// #include <WiFi.h>
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#include "config.h"
#include "ac_control.h"
#include <WiFiManager.h> 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String userPath;
unsigned long lastFirebaseCheck = 0;


String createPath(String base, String child) {
  String result = base;
  result += child;
  return result;
}
// Tambahkan variabel last untuk fan dan swing
int lastProtocol = -1, lastTemp = -1, lastFan = -1, lastMode = -1;
bool lastPower = false, lastSwing = false, isFirstRun = true;

// Variabel untuk reset wifi ke setelan pabrik
const int RESET_PIN = 0; // Pin untuk reset ke setelan pabrik
unsigned long pressStartTime = 0;
bool isPressing = false;

// Variabel untuk interval pengecekan Firebase
unsigned long lastFirebaseCheck = 0;
const int FIREBASE_CHECK_INTERVAL = 2000; // Cek setiap 2 detik

void setup() {
    Serial.begin(115200);
    setupAC();
    // Inisialisasi pin reset sebagai input 
    pinMode(RESET_PIN, INPUT_PULLUP);  
    // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    // Serial.println("\nWiFi Connected!");
    // userPath = "ac_system/settings";
    // WiFiManager
    WiFiManager wm;
    // Jika gagal konek ke WiFi lama, buat hotspot "SmartRemote_Setup"
    bool res = wm.autoConnect("SmartRemote_Setup"); 

    if(!res) {
        Serial.println("Gagal terhubung, restart...");
        ESP.restart();
    } 

    Serial.println("Terhubung ke WiFi!");

    // 2. Ambil ID Alat
    String deviceID = WiFi.macAddress();
    deviceID.replace(":", "");
    // 3. CARA AMAN: Gunakan Serial.print terpisah (Menghindari error '+')
    Serial.print("ID Alat Anda: ");
    Serial.println(deviceID);

    userPath = "/";
    userPath += deviceID;
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
    // logika reset wifi ke setelan pabrik
    if (digitalRead(RESET_PIN) == LOW) {
        if (!isPressing) {
            // Tombol baru ditekan
            pressStartTime = millis();
            isPressing = true;
            Serial.println("Tombol Boot ditekan, tahan 5 detik untuk reset WiFi...");
        } else {
            // Tombol masih ditekan, cek durasi tekan
            if (millis() - pressStartTime >= 5000) {
                Serial.println("Mereset pengaturan WiFi...");
                WiFiManager wm;
                wm.resetSettings();
                Serial.println("Pengaturan WiFi direset. Restarting...");
                delay(1000);
                ESP.restart();
            }
          }
        } else {
        // Tombol dilepas sebelum 5 detik
        if(isPressing) {
            Serial.println("Reset dibatalkan (tombol dilepas sebelum 5 detik).");
            isPressing = false;
        }
    }

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

