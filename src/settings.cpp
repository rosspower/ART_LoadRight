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

        prefs.putInt("minPDist", 5);
        prefs.putInt("maxPDist", 100);
        prefs.putInt("minSDist", 5);
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
    
    prefs.putInt("minPDist", savedsettings.minPDist);
    prefs.putInt("maxPDist", savedsettings.maxPDist);
    prefs.putInt("minSDist", savedsettings.minSDist);
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
    
    savedsettings.minPDist = prefs.getInt("minPDist", 5);
    savedsettings.maxPDist = prefs.getInt("maxPDist", 100);
    savedsettings.minSDist = prefs.getInt("minSDist", 5);
    savedsettings.maxSDist = prefs.getInt("maxSDist", 100);
    
    savedsettings.alertPPercent = prefs.getInt("alertPPercent", 20);
    savedsettings.alertSPercent = prefs.getInt("alertSPercent", 20);

    savedsettings.currentRndCount = prefs.getInt("currentRndCount", 0);
    savedsettings.totalRndCnt = prefs.getInt("totalRndCnt", 0);
            
    savedsettings.showMM = prefs.getBool("showMM", true);
    savedsettings.showShot = prefs.getBool("showShot", true);
    savedsettings.showCounter = prefs.getBool("showCounter", true);
    prefs.end();
}