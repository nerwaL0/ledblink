#include "firebase_manager.h"
#include <FirebaseESP32.h>
#include "config.h"
#include "ac_control.h"
#include "network_manager.h"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String userPath;
unsigned long lastFirebaseCheck = 0;

// Variabel status terakhir (Encapsulated)
int lastProtocol = -1, lastTemp = -1, lastFan = -1, lastMode = -1;
bool lastPower = false, lastSwing = false, isFirstRun = true;

String createPath(String base, String child) {
  String result = base;
  result += child;
  return result;
}

void setupFirebase() {
    userPath = "/devices/";
    userPath += getDeviceID();
    userPath += "/settings";
    
    Serial.print("Menghubungkan ke Firebase Path: ");
    Serial.println(userPath);

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    config.signer.test_mode = true;
    
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void handleFirebaseUpdates() {
    // Interval cek setiap 2 detik sesuai spesifikasi latensi sistem [cite: 109]
    if (millis() - lastFirebaseCheck < 2000) return;
    lastFirebaseCheck = millis();

    if (Firebase.ready()) {
        // ambil seluruh objek settings sekaligus
        if (Firebase.getJSON(fbdo, userPath.c_str())) {
            FirebaseJson &json = fbdo.jsonObject();
            FirebaseJsonData result;

            // Parsing data
            json.get(result, "protocol_id"); int cProtocol = result.intValue;
            json.get(result, "power"); bool cPower = result.boolValue;
            json.get(result, "temp"); int cTemp = result.intValue;
            json.get(result, "fan_speed"); int cFan = result.intValue;
            json.get(result, "swing"); bool cSwing = result.boolValue;
            json.get(result, "mode"); int cMode = result.intValue;

            // logika pengecekan data
            if (isFirstRun || cProtocol != lastProtocol || cPower != lastPower || 
                cTemp != lastTemp || cFan != lastFan || cSwing != lastSwing || cMode != lastMode) {
                
                tembakSinyalAC(cProtocol, cPower, cTemp, cFan, cSwing, cMode);
                
                lastProtocol = cProtocol; 
                lastPower = cPower; 
                lastTemp = cTemp;
                lastFan = cFan; 
                lastSwing = cSwing;
                lastMode = cMode;
                isFirstRun = false;
            }
        } else {
            Serial.print(F("Firebase error: "));
            Serial.println(fbdo.errorReason());
        }
    }
}
void uploadSensorData(float temp, float hum) {
    if (Firebase.ready()) {
        if (Firebase.setFloat(fbdo, createPath(userPath, "/read_temp").c_str(), temp)) {
            Serial.println(F("Data Suhu berhasil diunggah."));
     }
        if (Firebase.setFloat(fbdo, createPath(userPath, "/read_hum").c_str(), hum)) {
            Serial.println(F("Data Kelembapan berhasil diunggah."));
        }
    }
}