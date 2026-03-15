#include "ac_control.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Import semua AC populer di Indonesia
#include <ir_Daikin.h> //14
#include <ir_Panasonic.h> //3
#include <ir_Sharp.h> //27
#include <ir_LG.h> //6
#include <ir_Samsung.h> //11
#include <ir_Gree.h> //32

const uint16_t kIrLed = 14;

// Inisialisasi objek masing-masing merek
IRDaikinESP acDaikin(kIrLed);
IRPanasonicAc acPanasonic(kIrLed);
IRSharpAc acSharp(kIrLed);
IRLgAc acLG(kIrLed);
IRSamsungAc acSamsung(kIrLed);
IRGreeAC acGree(kIrLed);

void setupAC() {
    acDaikin.begin();
    acPanasonic.begin();
    acSharp.begin();
    acLG.begin();
    acSamsung.begin();
    acGree.begin();
}

void tembakSinyalAC(int id, bool powerStatus, int temp, int fan, bool swing, int mode) {
    Serial.printf("\n--- TRANSMIT: ID:%d, Pwr:%d, Temp:%d, Fan:%d, Swing:%d, Mode:%d ---\n", id, powerStatus, temp, fan, swing, mode);

    switch (id) {
        case 14: // DAIKIN
            if (powerStatus) {
                acDaikin.on();
                acDaikin.setTemp(temp);
                // Mode Mapping
                if (mode == 0) acDaikin.setMode(kDaikinAuto);
                else if (mode == 2) acDaikin.setMode(kDaikinDry);
                else if (mode == 3) acDaikin.setMode(kDaikinFan);
                else acDaikin.setMode(kDaikinCool); // Default ke Cool
                // Mapping Fan Speed Daikin
                if (fan == 0) acDaikin.setFan(kDaikinFanAuto);
                else if (fan == 1) acDaikin.setFan(1); // Low
                else if (fan == 2) acDaikin.setFan(3); // Med
                else acDaikin.setFan(5);              // High
                
                acDaikin.setSwingVertical(swing);
            } else {
                acDaikin.off();
            }
            acDaikin.send();
            break;
        case 3: // PANASONIC
            if (powerStatus) {
                acPanasonic.on();
                acPanasonic.setTemp(temp);
                // Mode Mapping
                if (mode == 0) acPanasonic.setMode(kPanasonicAcAuto);
                else if (mode == 2) acPanasonic.setMode(kPanasonicAcDry);
                else if (mode == 3) acPanasonic.setMode(kPanasonicAcFan);
                else acPanasonic.setMode(kPanasonicAcCool); // Default ke Cool
                // Mapping Fan Speed Panasonic
                if (fan == 0) acPanasonic.setFan(kPanasonicAcFanAuto);
                else if (fan == 1) acPanasonic.setFan(kPanasonicAcFanMin);
                else if (fan == 2) acPanasonic.setFan(kPanasonicAcFanMed);
                else acPanasonic.setFan(kPanasonicAcFanMax);

                acPanasonic.setSwingVertical(swing ? kPanasonicAcSwingVAuto : kPanasonicAcSwingVHighest);
            } else {
                acPanasonic.off();
            }
            acPanasonic.send();
            break;
       case 27: // SHARP
            if (powerStatus) {
                acSharp.on();
                acSharp.setMode(kSharpAcCool); // Paksa ke mode Cool
                acSharp.setTemp(temp);
                // Mode Mapping
                if (mode == 0) acSharp.setMode(kSharpAcAuto);
                else if (mode == 2) acSharp.setMode(kSharpAcDry);
                else if (mode == 3) acSharp.setMode(kSharpAcFan);
                else acSharp.setMode(kSharpAcCool); // Default ke Cool
                // Mapping Fan Speed Sharp
                if (fan == 0) acSharp.setFan(kSharpAcFanAuto);
                else if (fan == 1) acSharp.setFan(kSharpAcFanMin);
                else if (fan == 2) acSharp.setFan(kSharpAcFanMed);
                else acSharp.setFan(kSharpAcFanMax);

                acSharp.setSwingV(swing);
            } else {
                acSharp.off();
            }
            acSharp.send();
            break;
        case 6:  // LG
            if (powerStatus) {
                acLG.on();
                acLG.setMode(kLgAcCool); // Penting: Agar suhu bisa berubah
                acLG.setTemp(temp);
                // Mode Mapping
                if (mode == 0) acLG.setMode(kLgAcAuto);
                else if (mode == 2) acLG.setMode(kLgAcDry);
                else if (mode == 3) acLG.setMode(kLgAcFan);
                else acLG.setMode(kLgAcCool); // Default ke Cool
                // Mapping Fan Speed LG
                if (fan == 0) acLG.setFan(kLgAcFanAuto);
                else if (fan == 1) acLG.setFan(kLgAcFanLowest);
                else if (fan == 2) acLG.setFan(kLgAcFanMedium);
                else acLG.setFan(kLgAcFanHigh);

                acLG.setSwingV(swing); 
            } else {
                acLG.off();
            }
            acLG.send();
            break;
        case 11: // SAMSUNG
            if (powerStatus) {
                acSamsung.on();
                acSamsung.setMode(kSamsungAcCool);
                acSamsung.setTemp(temp);
                // Mode Mapping
                if (mode == 0) acSamsung.setMode(kSamsungAcAuto);
                else if (mode == 2) acSamsung.setMode(kSamsungAcDry);
                else if (mode == 3) acSamsung.setMode(kSamsungAcFan);
                else acSamsung.setMode(kSamsungAcCool); // Default ke Cool
                // Mapping Fan Speed Samsung
                if (fan == 0) acSamsung.setFan(kSamsungAcFanAuto);
                else if (fan == 1) acSamsung.setFan(kSamsungAcFanLow);
                else if (fan == 2) acSamsung.setFan(kSamsungAcFanMed);
                else acSamsung.setFan(kSamsungAcFanHigh);

                acSamsung.setSwing(swing);
            } else {
                acSamsung.off();
            }
            acSamsung.send();
            break;
        case 32: // GREE
    if (powerStatus) {
        acGree.on();
        acGree.setMode(kGreeCool); // Kunci di mode Cool
        acGree.setTemp(temp);      // Set suhu dinamis
        // Mode Mapping
        if (mode == 0) acGree.setMode(kGreeAuto);
        else if (mode == 2) acGree.setMode(kGreeDry);
        else if (mode == 3) acGree.setMode(kGreeFan);
        else acGree.setMode(kGreeCool); // Default ke Cool
        // Mapping Fan Speed Gree (0-3)
        switch(fan) {
            case 0: acGree.setFan(kGreeFanAuto); break;
            case 1: acGree.setFan(kGreeFanMin); break;
            case 2: acGree.setFan(kGreeFanMed); break;
            case 3: acGree.setFan(kGreeFanMax); break;
            default: acGree.setFan(kGreeFanAuto); break;
        }

        // Swing Vertical
        acGree.setSwingVertical(swing, kGreeSwingAuto);
    } else {
        acGree.off();
    }
    acGree.send();
    break;
        default:
            Serial.println("Protocol ID tidak terdaftar!");
            return;
    }
    Serial.println("Sinyal Berhasil Terkirim!");
    delay(200); // Delay kecil untuk memastikan sinyal terkirim sempurna sebelum perintah berikutnya
}