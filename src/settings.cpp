#include <Preferences.h>
#include <Arduino.h>
#include "settings.h"

Preferences prefs;

settings savedsettings;

void clearAllPreferences() { 
  Serial.println("clearing all eeprom");
  // nvs_flash_erase(); // erase the NVS partition and...
  // nvs_flash_init(); // initialize the NVS partition.
  // Begin Preferences with the current dynamic namespace
  prefs.begin("my-app");
  prefs.clear();
  prefs.end();
}


void initPreferences(){
    prefs.begin("my-app"); // use "my-app" namespace
    //prefs.clear();
    if (not prefs.isKey("settings")) {
        Serial.println("writting preferences");
        prefs.putBool("settings", true);
        prefs.putString("device_name", "AImRight SmartLoader");
        prefs.putString("device_desc", "An Intelligent Reloading System");
        prefs.putString("mdns_name", "SmartLoader");
        prefs.putBool("alarmEnabled", true);

        prefs.putInt("minpDist", 5);
        prefs.putInt("maxPDist", 100);
        prefs.putInt("minpDist", 5);
        prefs.putInt("maxSDist", 100);
       
        prefs.putInt("alertPPercent", 20);
        prefs.putInt("alertSPercent", 20);

        prefs.putInt("currentRndCount", 0);
        prefs.putInt("totalRndCnt", 0);
               
        prefs.putBool("showMM", true);
        prefs.putBool("showShot", true);
        prefs.putBool("showCounter", true);
        prefs.end();
    }
   prefs.end();
}

void storePreferences(){
    prefs.begin("my-app");
    Serial.println("inside storePreferences");
    prefs.putString("device_name", savedsettings.device_name);
    prefs.putString("device_desc", savedsettings.device_desc);
    prefs.putString("mdns_name", savedsettings.mdns_name);

    prefs.putBool("alarmEnabled", savedsettings.alarmEnabled);
    
    prefs.putInt("minpDist", savedsettings.minPDist);
    prefs.putInt("maxPDist", savedsettings.maxPDist);
    prefs.putInt("minpDist", savedsettings.minSDist);
    prefs.putInt("maxSDist", savedsettings.maxSDist);
    
    prefs.putInt("alertPPercent", savedsettings.alertPPercent);
    prefs.putInt("alertSPercent", savedsettings.alertSPercent);

    prefs.putInt("currentRndCount", savedsettings.currentRndCount);
    prefs.putInt("totalRndCnt", savedsettings.totalRndCnt);
            
    prefs.putBool("showMM", savedsettings.showMM);
    prefs.putBool("showShot", savedsettings.showShot);
    prefs.putBool("showCounter", savedsettings.showCounter);

    prefs.end();
}

void getPreferences(){
    prefs.begin("my-app");
    savedsettings.device_name = prefs.getString("device_name", "aimright smartloader");
    savedsettings.device_desc = prefs.getString("device_desc", "");
    savedsettings.mdns_name = prefs.getString("mdns_name", "smartLoader");
    savedsettings.alarmEnabled = prefs.getBool("alarmEnabled", true);
    
    savedsettings.minPDist = prefs.putInt("minpDist", savedsettings.minPDist);
    savedsettings.maxPDist = prefs.putInt("maxPDist", savedsettings.maxPDist);
    savedsettings.minSDist = prefs.putInt("minpDist", savedsettings.minSDist);
    savedsettings.maxSDist = prefs.putInt("maxSDist", savedsettings.maxSDist);
    
    savedsettings.alertPPercent = prefs.putInt("alertPPercent", savedsettings.alertPPercent);
    savedsettings.alertSPercent = prefs.putInt("alertSPercent", savedsettings.alertSPercent);

    savedsettings.currentRndCount = prefs.putInt("currentRndCount", savedsettings.currentRndCount);
    savedsettings.totalRndCnt = prefs.putInt("totalRndCnt", savedsettings.totalRndCnt);
            
    savedsettings.showMM = prefs.putBool("showMM", true);
    savedsettings.showShot = prefs.putBool("showShot", true);
    savedsettings.showCounter = prefs.putBool("showCounter", true);
    prefs.end();
}