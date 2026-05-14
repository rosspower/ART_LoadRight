#include <Preferences.h>
#include <Arduino.h>
#include "settings.h"

Preferences prefs;

settings savedsettings;
//needed for savedsettings
String nospaces(String str){
  str.replace(" ", "");
    return str;
}

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

        prefs.putString("device_name", project_name);
        prefs.putString("device_desc", project_name);
        prefs.putString("mdns_name", nospaces(project_name));
        prefs.putString("ssid_name", project_name);
        prefs.putString("ssid_pass", project_name);
        prefs.putInt("wifiMode", 2); // 0:none, 1:connected, 2:standalone

        prefs.putBool("settings", true);
        prefs.putString("device_name", "AImRight Technologies SmartLoader");
        prefs.putString("device_desc", "An Intelligent Reloading System");
        prefs.putString("mdns_name", "SmartLoader");
        prefs.putBool("alarmEnabled", true);

        prefs.putInt("calPDist", 0);
        prefs.putInt("calSDist", 0);
        prefs.putInt("minPDist", 20);
        prefs.putInt("maxPDist", 250);
        prefs.putInt("minSDist", 20);
        prefs.putInt("maxSDist", 250);
       
        prefs.putInt("alertPPercent", 20);
        prefs.putInt("alertSPercent", 20);

        prefs.putInt("currentRndCount", 0);
        prefs.putInt("totalRndCnt", 0);
               
        prefs.putBool("showMM", true);
        prefs.putBool("showShot", true);
        prefs.putBool("showCounter", true);
        prefs.putInt("brightness", 100);
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
    
    prefs.putString("ssid_name", savedsettings.ssid_name);
    prefs.putString("ssid_pass", savedsettings.ssid_password);
    prefs.putInt("wifiMode", savedsettings.wifiMode);
    prefs.putBool("alarmEnabled", savedsettings.alarmEnabled);

    prefs.putInt("calPDist", savedsettings.calPDist);
    prefs.putInt("calSDist", savedsettings.calSDist);
  
    
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
    prefs.putInt("brightness", savedsettings.brightness);

    prefs.end();
}

void getPreferences(){
    prefs.begin("my-app");
    savedsettings.device_name = prefs.getString("device_name", "aimright smartloader");
    savedsettings.device_desc = prefs.getString("device_desc", "");
    savedsettings.mdns_name = prefs.getString("mdns_name", nospaces(project_name));
    savedsettings.ssid_name = prefs.getString("ssid_name", project_name);
    savedsettings.ssid_password = prefs.getString("ssid_pass", project_name);
    savedsettings.wifiMode = prefs.getInt("wifiMode", 0);
    savedsettings.alarmEnabled = prefs.getBool("alarmEnabled", true);

    savedsettings.calPDist = prefs.getInt("calPDist", 0);
    savedsettings.calSDist = prefs.getInt("calSDist", 0);
    
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
    savedsettings.brightness = prefs.getInt("brightness", 127);
    prefs.end();
}