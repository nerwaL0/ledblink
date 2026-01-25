#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>

// Inisialisasi WiFi dan WiFiManager
void setupNetwork();

// Fungsi untuk mengecek apakah tombol BOOT ditahan untuk reset
void checkResetButton();

// Fungsi untuk mendapatkan ID unik (MAC Address)
String getDeviceID();

#endif