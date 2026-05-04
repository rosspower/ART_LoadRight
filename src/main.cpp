

/*  Install the "lvgl" library version 9.2 by kisvegabor to interface with the TFT Display - https://lvgl.io/
    *** IMPORTANT: lv_conf.h available on the internet will probably NOT work with the examples available at Random Nerd Tutorials ***
    *** YOU MUST USE THE lv_conf.h FILE PROVIDED IN THE LINK BELOW IN ORDER TO USE THE EXAMPLES FROM RANDOM NERD TUTORIALS ***
    FULL INSTRUCTIONS AVAILABLE ON HOW CONFIGURE THE LIBRARY: https://RandomNerdTutorials.com/cyd-lvgl/ or https://RandomNerdTutorials.com/esp32-tft-lvgl/   */
#include <Arduino.h>    
#include <lvgl.h>

/*  Install the "TFT_eSPI" library by Bodmer to interface with the TFT Display - https://github.com/Bodmer/TFT_eSPI
    *** IMPORTANT: User_Setup.h available on the internet will probably NOT work with the examples available at Random Nerd Tutorials ***
    *** YOU MUST USE THE User_Setup.h FILE PROVIDED IN THE LINK BELOW IN ORDER TO USE THE EXAMPLES FROM RANDOM NERD TUTORIALS ***
    FULL INSTRUCTIONS AVAILABLE ON HOW CONFIGURE THE LIBRARY: https://RandomNerdTutorials.com/cyd-lvgl/ or https://RandomNerdTutorials.com/esp32-tft-lvgl/   */
#include <TFT_eSPI.h>

// Install the "XPT2046_Touchscreen" library by Paul Stoffregen to use the Touchscreen - https://github.com/PaulStoffregen/XPT2046_Touchscreen - Note: this library doesn't require further configuration
#include <XPT2046_Touchscreen.h>

#include "UI\ui.h"
#include "settings.h"

//network includes
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Preferences.h>
#include <ESPmDNS.h>
#include <ESP32httpUpdate.h>
#include "ESP32S3Buzzer.h"
#include "VL53L1X_ULD.h"
#include <ArduinoJson.h>



// CYD often uses 27 for SDA and 22 for SCL
#define I2C_SDA 27
#define I2C_SCL 22

VL53L1X_ULD powderSensor;
VL53L1X_ULD shotSensor;

#define powderSensor_XSHUT 17
#define shotSensor_I2C_ADDRESS 0x55

// Allocate the JSON document
JsonDocument doc;



// Set the buzzer pin and channel
const uint8_t buzzerPin = 27;
const uint8_t buzzerChannel = 0;

// Create an instance of ESP32S3Buzzer
ESP32S3Buzzer buzzer(buzzerPin, buzzerChannel);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Connect to Wi-Fi
WiFiManager wm;

const int BACKLIGHT_PIN = 21;
const int LDRPIN = 34; // LDR sensor

// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

int update_interval = 100;
// int alarmDelay = 5 * 60 * 1000;
int alarmDelay = 10000;








// Measurement Declarations for Sensor
//int alarmEnabled = 1;
int rawPValue = 20; // Measurement from the sensor in mm
int rawSValue = 20; // Measurement from the sensor in mm
int minDist = 20;  // The closest reading to the sensor in mm: 100% full
int maxDist = 120; // The furthest reading from the sensor in mm: 0% full
int lastPercent = 0;
int newPercent;
int alertPPercent = 0;
int warnPPercent = 0;
int alertSPercent = 0;
int warnSPercent = 0;
int grainsPerMM = 12;
int grainsLeft = 0;
int grainsAdded = 50; // For measuring grains per mm
int grainsAddedStart = 0;
int grainsAddedFinish = 0;
int alertPercentGrains = 0;
int currentRndCount = 0;
long lastupdate = 0;
long lastAlarmSilenced = 0;
int pValue = 100;
int sValue = 100;
bool isAlarmSilenced = false;
bool isAlarmed = false;
bool hasi2Device = false;
bool hasPowderSensor = false;
bool hasShotSensor = false;
// bool showMM = true;
// bool showShot = true;
// bool showCounter = true;






// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];



void setSwitch(lv_obj_t * obj, bool state){
  if (state == 1){
    lv_obj_add_state(obj, LV_STATE_CHECKED);
  } else {
    lv_obj_clear_state(obj, LV_STATE_CHECKED);
  }
}

void setUiElementState(lv_obj_t * obj, bool state){
  if (state == 1){
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
  }
}

void setUILabelText(lv_obj_t * obj, String inText){
  lv_label_set_text(obj, inText.c_str());
}

void getSensorValues(){
  if (hasPowderSensor){
     pValue = map(rawPValue, savedsettings.minPDist, savedsettings.maxPDist, 100, 0 );
    if (pValue < 0) pValue = 0;
    if (pValue > 100) pValue = 100;
  }
 

  //read raw svalue from sensor
  rawSValue = random(savedsettings.minSDist, savedsettings.maxSDist);
  sValue = map(rawSValue, savedsettings.minSDist, savedsettings.maxSDist, 100, 0 );

}








// If logging is enabled, it will inform the user about what is happening in the library
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

// Get the Touchscreen data
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z)
  if(touchscreen.tirqTouched() && touchscreen.touched()) {
    // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;
    data->state = LV_INDEV_STATE_PRESSED;
    // Set the coordinates
    data->point.x = x;
    data->point.y = y;
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void playBuzzer(){
  // Add a tone to the queue
  uint32_t freq = 1000;
  uint32_t onDuration = 200;
  uint32_t offDuration = 200;
  uint16_t cycles = 5;
  buzzer.tone(freq, onDuration, offDuration, cycles);
}



void setUiTextColor(lv_obj_t * obj, String inmode){
  int setColor = 0xffffff;
  if (inmode == "alert"){
    setColor = 0xFF0000;
  } else if (inmode == "warn"){
    setColor = 0xFF4D01;
  }
  lv_obj_set_style_text_color(obj, lv_color_hex(setColor), LV_PART_MAIN);
}



void updateDisplayValues(){
  setSwitch(ui_EnableAlarmSwitch, savedsettings.alarmEnabled);
  // showmm
  setSwitch(ui_showDistanceSwitch, savedsettings.showMM);
  setUiElementState(ui_powderdistValueLabel, savedsettings.showMM);
  setUiElementState(ui_shotDistValueLabel, savedsettings.showMM);

  //showShot
  setSwitch(ui_showShotSwitch, savedsettings.showShot);
  setUiElementState(ui_ShotIndicator, savedsettings.showShot);
  if (hasShotSensor == false) setUiElementState(ui_ShotIndicator, 0);

  //showCounter
  setSwitch(ui_showCounterSwitch, savedsettings.showCounter);
  setUiElementState(ui_CounterContainer, savedsettings.showCounter);

   
  
  setUILabelText(ui_maxPDist, String(savedsettings.maxPDist));
  setUILabelText(ui_minPDist, String(savedsettings.minPDist));

  setUILabelText(ui_maxSDist, String(savedsettings.maxSDist));
  setUILabelText(ui_minSDist, String(savedsettings.minSDist));

  lv_dropdown_set_selected(ui_wifiModeSelect, savedsettings.wifiMode);

  lv_slider_set_value(ui_brightnessSlider, savedsettings.brightness, LV_ANIM_OFF);

}

void updateShotRTDisplay(){
    String tmpstring = String(rawSValue) + " mm";
  setUILabelText(ui_shotDistValueLabel, tmpstring);

  //calibrate powder height distance 
  setUILabelText(ui_ShotCurrentDistance, String(rawSValue));

  int sliderValue = sValue;
  if (sliderValue < 3) sliderValue = 3;

  lv_slider_set_value(ui_shotSlider, sliderValue, LV_ANIM_ON);
  String tempStr = String(sValue) + "%"; 
  const char* myStr = tempStr.c_str(); // Get the underlying const char*
  lv_label_set_text(ui_ShotValueLabel, myStr);
  // tempStr = String(currentRndCount * 100); 
  // myStr = tempStr.c_str(); // Get the underlying const char*
  // lv_label_set_text(ui_roundCounterLabel, myStr);
  
  if (sValue <= alertSPercent){
    if ((savedsettings.alarmEnabled == true) && (isAlarmSilenced == false)){
      setUiElementState(ui_alarmImage1, 1);
      playBuzzer();
      
    } else {
      setUiElementState(ui_alarmImage1, 0);  
    }

    lv_obj_set_style_bg_color(ui_shotSlider, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
    setUiTextColor(ui_ShotValueLabel, "alert");
    setUiTextColor(ui_Shot_Label, "alert");

    // lv_obj_set_style_text_color(ui_powderValueLabel, lv_color_hex(0xFF0000), LV_PART_MAIN);
  } else if(sValue <= warnSPercent){
    if (savedsettings.alarmEnabled == true){
      setUiElementState(ui_alarmImage1, 0);
      
    }  
    lv_obj_set_style_bg_color(ui_shotSlider, lv_color_hex(0xFF5C00), LV_PART_INDICATOR);
    setUiTextColor(ui_ShotValueLabel, "warn");
    setUiTextColor(ui_Shot_Label, "warn");
    //lv_obj_set_style_text_color(ui_powderValueLabel, lv_color_hex(0xFF5C00), LV_PART_MAIN);
  }
  else {
    if (savedsettings.alarmEnabled == true){
      setUiElementState(ui_alarmImage1, 0);
    }
    lv_obj_set_style_bg_color(ui_shotSlider, lv_color_hex(0x127612), LV_PART_INDICATOR);
    setUiTextColor(ui_ShotValueLabel, "norm");
    setUiTextColor(ui_Shot_Label, "norm");
   
    //lv_obj_set_style_text_color(ui_powderValueLabel, lv_color_hex(0xffffff), LV_PART_MAIN);
  }
}

void updatePowderRTDisplay(){
   // update live data

  String tmpstring = String(rawPValue) + " mm";
  setUILabelText(ui_powderdistValueLabel, tmpstring);

  //calibrate powder height distance 
  setUILabelText(ui_PowderCurrentDistance, String(rawPValue));

  int sliderValue = pValue;
  if (sliderValue < 3) sliderValue = 3;

  lv_slider_set_value(ui_powderSlider, sliderValue, LV_ANIM_ON);
  String tempStr = String(pValue) + "%"; 
  const char* myStr = tempStr.c_str(); // Get the underlying const char*
  lv_label_set_text(ui_powderValueLabel, myStr);
  tempStr = String(currentRndCount * 100); 
  myStr = tempStr.c_str(); // Get the underlying const char*
  lv_label_set_text(ui_roundCounterLabel, myStr);
  
  if (pValue <= alertPPercent){
    if ((savedsettings.alarmEnabled == true) && (isAlarmSilenced == false)){
      setUiElementState(ui_alarmImage, 1); 
      playBuzzer();
      isAlarmed = true; 
    } else {
      setUiElementState(ui_alarmImage, 0);  
    }

    lv_obj_set_style_bg_color(ui_powderSlider, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
    setUiTextColor(ui_powderValueLabel, "alert");
    setUiTextColor(ui_Powder_label, "alert");

    // lv_obj_set_style_text_color(ui_powderValueLabel, lv_color_hex(0xFF0000), LV_PART_MAIN);
  } else if(pValue <= warnPPercent){
    if (savedsettings.alarmEnabled == true){
      setUiElementState(ui_alarmImage, 0);
    }  
    lv_obj_set_style_bg_color(ui_powderSlider, lv_color_hex(0xFF5C00), LV_PART_INDICATOR);
    setUiTextColor(ui_powderValueLabel, "warn");
    setUiTextColor(ui_Powder_label, "warn");
    //lv_obj_set_style_text_color(ui_powderValueLabel, lv_color_hex(0xFF5C00), LV_PART_MAIN);
  }
  else {
    if (savedsettings.alarmEnabled == true){
      setUiElementState(ui_alarmImage, 0);
    }
    lv_obj_set_style_bg_color(ui_powderSlider, lv_color_hex(0x127612), LV_PART_INDICATOR);
    setUiTextColor(ui_powderValueLabel, "norm");
    setUiTextColor(ui_Powder_label, "norm");
    //lv_obj_set_style_text_color(ui_powderValueLabel, lv_color_hex(0xffffff), LV_PART_MAIN);
  }

}

void updateAlerts(){
  alertPPercent = savedsettings.alertPPercent;
  warnPPercent = alertPPercent  * 1.5;
  alertSPercent = savedsettings.alertSPercent;
  warnSPercent = alertSPercent  * 1.5;
}

void updateRealTimeDisplay(){
  if((pValue <= alertPPercent) && (savedsettings.alarmEnabled == true)){
    isAlarmed = true;
  } else if ((sValue <= alertSPercent) && (hasShotSensor == true)){
    isAlarmed = true;
  } else {
    isAlarmed = false;
  }
  updatePowderRTDisplay();
  if((savedsettings.showShot == true) && (hasShotSensor))
  updateShotRTDisplay();
 

  

}

void updateDataDisplay(){
  storePreferences();
  getPreferences();
  updateAlerts();
  updateDisplayValues();
  updateRealTimeDisplay();
}


void reset_counter() {
  currentRndCount = 0;
  lv_obj_add_flag(ui_resetCounterPanel, LV_OBJ_FLAG_HIDDEN);    // Hide
}

void set_device_name(String setting){
  savedsettings.device_name = setting;
  storePreferences();
}

void set_device_desc(String setting){
  savedsettings.device_desc = setting;
  storePreferences();
}

void setAlarmEnabled(int input){
  savedsettings.alarmEnabled = input;
  storePreferences();
}

void set_mdns_name(String setting){
  savedsettings.mdns_name = setting;
  storePreferences();
  ESP.restart();
}

void setWifiMode(int mode){
  savedsettings.wifiMode = mode;
  storePreferences();
  delay(500);
  ESP.restart();
}

extern "C" {

   void wifiModeSelectCallBack(lv_event_t * e) {
    Serial.println(lv_dropdown_get_selected(ui_wifiModeSelect));
    savedsettings.wifiMode =  lv_dropdown_get_selected(ui_wifiModeSelect);
    setWifiMode(savedsettings.wifiMode);
   
    
    
  }


  void measurePEmptyLevelCallBack(lv_event_t * e) {
    //set powder max level
    savedsettings.maxPDist = rawPValue;
    updateDataDisplay();
  }

  void measurePFullLevelCallBack(lv_event_t * e) {
    //set powder max level
    savedsettings.minPDist = rawPValue;
    updateDataDisplay();
  }

  void measureSEmptyLevelCallBack(lv_event_t * e) {
    //set powder max level
    savedsettings.maxSDist = 138;
    updateDataDisplay();
  }

  void measureSFullLevelCallBack(lv_event_t * e) {
    //set powder max level
    savedsettings.minSDist = 7;
    updateDataDisplay();
  }

  void brightnessSliderCallBack(lv_event_t * e) {
    int32_t sliderValue = lv_slider_get_value(ui_brightnessSlider);
    savedsettings.brightness = sliderValue;
    updateDataDisplay();
    int brightness = map(sliderValue, 0, 100, 50, 255); // Map, adjusting for 8-bit PWM
    analogWrite(BACKLIGHT_PIN, brightness);
  }

  void ClearAllSettingsCallback(lv_event_t * e) {
     clearAllPreferences();
     initPreferences();
     getPreferences();
     updateAlerts();
     updateDisplayValues();
    }

    void resetCounterCallBack(lv_event_t * e) {
      reset_counter();
    }

    void alarmImageClickCallBack(lv_event_t * e) {
      isAlarmSilenced = true;
      Serial.println("alarm silenced");
      setUiElementState(ui_alarmImage, 0);
      lastAlarmSilenced = millis();
    }

    void toggleAlertCallBack(lv_event_t * e) {
      savedsettings.alarmEnabled = lv_obj_has_state(ui_EnableAlarmSwitch, LV_STATE_CHECKED);
      updateDataDisplay();
    }

    void toggleShowDistanceCallBack(lv_event_t * e) {
      savedsettings.showMM = lv_obj_has_state(ui_showDistanceSwitch, LV_STATE_CHECKED);
      updateDataDisplay();
    }

    void toggleShowShotCallBack(lv_event_t * e) {
      savedsettings.showShot = lv_obj_has_state(ui_showShotSwitch, LV_STATE_CHECKED);
      updateDataDisplay();
    }

    void toggleShowCounterCallBack(lv_event_t * e) {
      savedsettings.showCounter = lv_obj_has_state(ui_showCounterSwitch, LV_STATE_CHECKED);
      updateDataDisplay();
    }



}

boolean i2cScanner(){
  bool result = false;
   byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  if (nDevices > 0){
    hasi2Device = true;
    result = true;
  }

  return result;
}
void sensorSetup(){
  digitalWrite(powderSensor_XSHUT, LOW);

  if (i2cScanner() == true){
    // Initialize sensor 2
    VL53L1_Error status = shotSensor.Begin(shotSensor_I2C_ADDRESS);
    if (status != VL53L1_ERROR_NONE) {
      // If the sensor could not be initialized print out the error code. -7 is timeout
      Serial.println("Could not initialize sensor 2, error code: " + String(status));
      // while (1) {}
    } else {
      Serial.println("Shot Sensor initialized");
      // Set the I2C address of sensor 2 to a different address as the default. 
      shotSensor.SetI2CAddress(shotSensor_I2C_ADDRESS);
      shotSensor.StartRanging();
      hasShotSensor = true;
    }
    
  } else {
    Serial.println("Did not find Shot Sensor after disabling Powder Sensor");
  }

  

  // Turn on sensor 1 by pulling the XSHUT pin HIGH
  digitalWrite(powderSensor_XSHUT, HIGH);

  // Initialize sensor 1
  VL53L1_Error status = powderSensor.Begin();
  if (status != VL53L1_ERROR_NONE) {
    // If the sensor could not be initialized print out the error code. -7 is timeout
    Serial.println("Could not initialize sensor 1, error code: " + String(status));
    // while (1) {}
  } else {
    Serial.println("Sensor initialized");
    hasPowderSensor = true;
    powderSensor.StartRanging();
  }

}

String getRSSI(){
  return String(WiFi.RSSI());
}

String getPercents(){
  doc["pValue"] = pValue;
  doc["sValue"] = sValue;
  doc["isAlarmSilenced"] = isAlarmSilenced;
  doc["isAlarmed"] = isAlarmed;
  doc["alarmDelay"] = alarmDelay;
  String returnString = "";
  serializeJson(doc, returnString);
  return returnString;
}

String getSettings() {
  getPreferences();
  updateAlerts();
  //testSD(); must put back
  doc["device_name"] = String(savedsettings.device_name);
  doc["device_desc"] = String(savedsettings.device_desc);
  doc["mdns_name"] = String(savedsettings.mdns_name);
  doc["wifi_mode"] = savedsettings.wifiMode;
  doc["ip_address"] = WiFi.localIP().toString();
  doc["alarmEnabled"] = savedsettings.alarmEnabled;
  doc["minDist_1"] = savedsettings.minPDist;
  doc["minDist_2"] = savedsettings.minSDist;
  doc["maxDist_1"] = savedsettings.maxPDist;
  doc["maxDist_2"] = savedsettings.maxSDist;
  doc["alertPercent_1"] = savedsettings.alertPPercent;
  doc["alertPercent_2"] = savedsettings.alertSPercent;
  //doc["grainsPerMM"] = savedsettings.grainsPerMM;
  doc["isAlarmSilenced"] = isAlarmSilenced;
  doc["isAlarmed"] = isAlarmed;
  doc["alarmDelay"] = alarmDelay;
  doc["dualMode"] = hasShotSensor;
  
 
  String returnString = "";
  serializeJson(doc, returnString);
  return returnString;
 }

void readPowderSensor(){
  // Checking if data is available. This can also be done through the hardware interrupt
  uint8_t dataReady_powderSensor = false;
  while(!dataReady_powderSensor) {
    powderSensor.CheckForDataReady(&dataReady_powderSensor);
    delay(5);
  }

  // Get the results
  uint16_t distance1, distance2;
  powderSensor.GetDistanceInMm(&distance1);
  //Serial.println("Powder Sensor Distance in mm: " + String(distance1));
  rawPValue = distance1;
  

  // After reading the results reset the interrupt to be able to take another measurement
  powderSensor.ClearInterrupt();
 
}

void readshotSensor(){
  // Checking if data is available. This can also be done through the hardware interrupt
  uint8_t dataReady_shotSensor = false;
  while(!dataReady_shotSensor) {
    shotSensor.CheckForDataReady(&dataReady_shotSensor);
    delay(5);
  }

  // Get the results
  uint16_t distance2;
  shotSensor.GetDistanceInMm(&distance2);
  Serial.println("Shot Sensor Distance in mm: " + String(distance2));

  // After reading the results reset the interrupt to be able to take another measurement
  shotSensor.ClearInterrupt();
}

void init_spiffs(){
        // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } else {
    Serial.println("Spiffs Loaded");
  }
 
}

void init_wifi(){
  //start the wifi manager
  wm.setTitle(project_name);
  wm.setCustomHeadElement("<style>h1:first-of-type { color: rgba(0,0,0,0);} h3:first-of-type {color: red;} </style>");
  wm.setHostname(nospaces(project_name));
  if (!wm.autoConnect(savedsettings.ssid_name.c_str(), savedsettings.ssid_password.c_str())){ // password protected ap) {
     
  } else {
    Serial.println("Wifi Connected");
    Serial.println("IP: " + WiFi.localIP().toString());
    setUILabelText(ui_ipAddressLabel, "IP Address: " + WiFi.localIP().toString());
  }

}

void init_AP(){
  WiFi.mode(WIFI_AP);
  WiFi.softAP(savedsettings.ssid_name,savedsettings.ssid_password);
  setUILabelText(ui_ipAddressLabel, "IP Address: 192.168.4.1" );
}



void init_pages(){
  server.on("/epwc.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/epwc.css", "text/css");
  });

  server.on("/common.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/common.js");
  });

  //   server.on("/menu.png", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(SPIFFS, "/menu.png", "menu/png");
  // });

  server.on("/epwc.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/epwc.css", "text/css");
  });

  server.on("/common.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/common.js");
  });

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
   server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send(SPIFFS, "/index.html");
  });

  server.on("/update.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/update.html");
  });

  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/settings.html");
  });

  server.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.png", "image/png");
  });

  server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/logo.png", "image/png");
  });

  server.on("/home.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/home.png", "image/png");
  });

  server.on("/gear.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/gear.png", "image/png");
  });

  server.on("/get_values", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getPercents().c_str());
  });

  server.on("/get_settings", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getSettings());
  });

  server.on("/rssi", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getRSSI().c_str());
  });

  server.on("/alarmSilence", HTTP_GET, [](AsyncWebServerRequest *request){
    isAlarmSilenced = true;
    Serial.println("alarm silenced");
    // setUiElementState(ui_alarmImage, 0);
    lastAlarmSilenced = millis();
    request->send(200, "text/plain", "OK");
  });

  

  server.on("/set_settings", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    Serial.println("got settings set!!!!!!!!!!!!");
    
   
    if (request->hasParam("device_name")) {
      inputMessage = request->getParam("device_name")->value();
      set_device_name(inputMessage);
    }

    if (request->hasParam("device_desc")) {
      inputMessage = request->getParam("device_desc")->value();
      set_device_desc(inputMessage);
    }

    if (request->hasParam("alertPercent_1")) {
      inputMessage = request->getParam("alertPercent_1")->value();
      savedsettings.alertPPercent = inputMessage.toInt();
      storePreferences();
    }

    if (request->hasParam("alertPercent_2")) {
      inputMessage = request->getParam("alertPercent_2")->value();
      savedsettings.alertSPercent = inputMessage.toInt();
      storePreferences();
    }

    if (request->hasParam("mdns_name")) {
      inputMessage = request->getParam("mdns_name")->value();
      set_mdns_name(inputMessage.c_str());
    }

    if (request->hasParam("wifi_mode")) {
      inputMessage = request->getParam("wifi_mode")->value();
      setWifiMode(inputMessage.toInt());
    }

     if (request->hasParam("alarmEnabled")) {
      inputMessage = request->getParam("alarmEnabled")->value();
      setAlarmEnabled(inputMessage.toInt());
    }
    request->redirect("/index.html");
    getPreferences();
    updateAlerts();
    
  });
}



void setup() {
  // Initialize the buzzer
  //buzzer.begin();
  // String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.begin(115200);
    // Turn off sensor 1 by pulling the XSHUT pin LOW
  pinMode(powderSensor_XSHUT, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(19, OUTPUT);

  digitalWrite(powderSensor_XSHUT, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(16, LOW);




  // Initialize I2C with specific pins
  Wire.begin(I2C_SDA, I2C_SCL);
  if (i2cScanner() == true){
    sensorSetup();  
  }
 
  
  
  // delay(4000);
  initPreferences();
  getPreferences();
  updateAlerts();
  init_spiffs();

    // Start LVGL
  lv_init();
  // Register print function for debugging
  lv_log_register_print_cb(log_print);

  // Start the SPI for the touchscreen and init the touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  // Set the Touchscreen rotation in landscape mode
  // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 0: touchscreen.setRotation(0);
  touchscreen.setRotation(0);

  // Create a display object
  lv_display_t * disp;
  // Initialize the TFT display using the TFT_eSPI library
  disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  // lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);
    
  // Initialize an LVGL input device object (Touchscreen)
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  // Set the callback function to read Touchscreen input
  lv_indev_set_read_cb(indev, touchscreen_read);
  ui_init();

  setUiElementState(ui_resetCounterPanel, 0);
  setUiElementState(ui_alarmImage, 0);
  setUiElementState(ui_alarmImage1, 0);
 

  // set initial brightness from prefs
  int brightness = map(savedsettings.brightness, 0, 100, 50, 255); // Map, adjusting for 8-bit PWM
  analogWrite(BACKLIGHT_PIN, brightness);
  updateDataDisplay();

  
  setUILabelText(ui_ipAddressLabel, "IP Address: None");

  if (savedsettings.wifiMode == 1){
    Serial.println("Connecting...");
    init_wifi();
    if (!MDNS.begin("SmartLoader")) {
      Serial.println("Error setting up MDNS responder!");
    } else {
      Serial.println("mDNS responder started");
    }
    server.begin();
    Serial.println("TCP server started");
    MDNS.addService("http", "tcp", 80);
    init_pages();
  } 
  else if (savedsettings.wifiMode == 2){
      // create an access point at 192.168.4.1
    Serial.println("Creating Access Point...");
    init_AP();
    //delay(1000);
    server.begin();
    Serial.println("TCP server started");
    init_pages();
    Serial.println("IP: " + WiFi.softAPIP().toString());
    //delay(3000);
   }






}




void sudoCounter(){
  currentRndCount +=1;
  if (currentRndCount * 100 > 99999) currentRndCount = 0;

}



void loop() {
    // Call the update method to process tones in the queue
  buzzer.update();
  lv_task_handler();  // let the GUI do its work
  lv_tick_inc(5);     // tell LVGL how much time has passed
  delay(5);           // let this time pass
  lv_timer_handler(); // Update the UI

  // static uint32_t last_time = 0;
  //   if (millis() - last_time > 500) {
  //       last_time = millis();
      
  //       pvalue -=1;
  //       currentRndCount +=1;
        
  //       updatePowder(pvalue);
  //       if (pvalue < 1) pvalue = 100;
  //       if (currentRndCount * 100 > 99999) currentRndCount = 0;
  //   }

    if (millis() - lastupdate > update_interval) {
        lastupdate = millis();
        getSensorValues();
        updateRealTimeDisplay();
        sudoCounter();
        if (hasPowderSensor == true){
           readPowderSensor();
        }
       
 
    }

    if (isAlarmSilenced == true){
      if (millis() - lastAlarmSilenced > alarmDelay) {
        isAlarmSilenced = false;
        Serial.println("alarm silence Ended");
      }
    }

  
}