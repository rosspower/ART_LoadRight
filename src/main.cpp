

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

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Connect to Wi-Fi
WiFiManager wm;

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

// Preferences prefs;

// struct settings {
//   String device_name;  // 1
//   String device_desc;  // 2
//   String mdns_name;
//   int alarmEnabled;
  
//   int minPDist;  // The closest reading to the sensor in mm: 100% full
//   int maxPDist; // The furthest reading from the sensor in mm: 0% full
//   int minSDist;  // The closest reading to the sensor in mm: 100% full
//   int maxSDist; // The furthest reading from the sensor in mm: 0% full
//   int alertPPercent;
//   int alertSPercent;
//   int currentRndCount;
//   int totalRndCnt;
//   bool showMM = 1;
//   bool showShot = true;
//   bool showCounter = true;
//  };

// settings savedsettings;


// void clearAllPreferences() { 
//   Serial.println("clearing all eeprom");
//   // nvs_flash_erase(); // erase the NVS partition and...
//   // nvs_flash_init(); // initialize the NVS partition.
//   // Begin Preferences with the current dynamic namespace
//   prefs.begin("my-app");
//   prefs.clear();
//   prefs.end();
// }


// void initPreferences(){
//     prefs.begin("my-app"); // use "my-app" namespace
//     //prefs.clear();
//     if (not prefs.isKey("settings")) {
//         Serial.println("writting preferences");
//         prefs.putBool("settings", true);
//         prefs.putString("device_name", "AImRight SmartLoader");
//         prefs.putString("device_desc", "An Intelligent Reloading System");
//         prefs.putString("mdns_name", "SmartLoader");
//         prefs.end();
//     }
//    prefs.end();
// }

// void storePreferences(){
//     prefs.begin("my-app");
//     Serial.println("inside storePreferences");
//     prefs.putString("device_name", savedsettings.device_name);
//     prefs.putString("device_desc", savedsettings.device_desc);
//     prefs.putString("mdns_name", savedsettings.mdns_name);
//     // prefs.putString("selected_motor",savedsettings.selected_motor);
//     // prefs.putInt("num_poles", savedsettings.num_poles);
//     // prefs.putInt("pass_kv", savedsettings.pass_kv);
//     // prefs.putInt("pass_current", savedsettings.pass_current);
//     // prefs.putInt("pass_rpm", savedsettings.pass_rpm);
//     // prefs.putString("sn_ident", savedsettings.sn_ident);
//     prefs.end();
// }

// void getPreferences(){
//     prefs.begin("my-app");
//     savedsettings.device_name = prefs.getString("device_name", "aimright smartloader");
//     savedsettings.device_desc = prefs.getString("device_desc", "");
//     savedsettings.mdns_name = prefs.getString("mdns_name", "smartLoader");
//     // savedsettings.selected_motor = prefs.getString("selected_motor", "none");
//     // savedsettings.num_poles = prefs.getInt("num_poles", 12);
//     // savedsettings.pass_kv = prefs.getInt("pass_kv", 0);
//     // savedsettings.pass_current = prefs.getInt("pass_current", 0);
//     // savedsettings.pass_rpm = prefs.getInt("pass_rpm", 0);
//     // savedsettings.sn_ident = prefs.getString("sn_ident", "123|123");
//     prefs.end();
// }





// Measurement Declarations for Sensor
int alarmEnabled = 1;
int rawPValue = 20; // Measurement from the sensor in mm
int rawSValue = 20; // Measurement from the sensor in mm
int minDist = 20;  // The closest reading to the sensor in mm: 100% full
int maxDist = 120; // The furthest reading from the sensor in mm: 0% full
int lastPercent = 0;
int newPercent;
int alertPPercent = 20;
int warnPPercent = alertPPercent  * 1.5;
int alertSPercent = 20;
int warnSPercent = alertSPercent  * 1.5;
int grainsPerMM = 12;
int grainsLeft = 0;
int grainsAdded = 50; // For measuring grains per mm
int grainsAddedStart = 0;
int grainsAddedFinish = 0;
int alertPercentGrains = 0;
int currentRndCount = 0;
bool showMM = true;
bool showShot = true;
bool showCounter = true;





// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];






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

    // Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
    // /* Serial.print("X = ");
    // Serial.print(x);
    // Serial.print(" | Y = ");
    // Serial.print(y);
    // Serial.print(" | Pressure = ");
    // Serial.print(z);
    // Serial.println();*/
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

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

void initDisplayValues(){

  setSwitch(ui_EnableAlarmSwitch, alarmEnabled);
  // showmm
  setSwitch(ui_showDistanceSwitch, showMM);
  setUiElementState(ui_powderdistValueLabel, showMM);
  setUiElementState(ui_shotDistValueLabel, showMM);

  //showShot
  setSwitch(ui_showShotSwitch, showShot);
  setUiElementState(ui_ShotIndicator, showShot);
  setUiElementState(ui_ShotIndicator, showShot);

  //showCounter
  setSwitch(ui_showCounterSwitch, showCounter);
  setUiElementState(ui_CounterContainer, showCounter);
  setUiElementState(ui_CounterContainer, showCounter);  
  



  
}


 void reset_counter() {
        // Serial.println("Button Clicked!");
        // Add your logic here (e.g., toggle an LED)
        currentRndCount = 0;
        lv_obj_add_flag(ui_resetCounterPanel, LV_OBJ_FLAG_HIDDEN);    // Hide
        // lv_obj_remove_flag(panel, LV_OBJ_FLAG_HIDDEN); // Show
   
}

extern "C" {
    void resetCounterCallBack(lv_event_t * e) {
      reset_counter();
    }
}

extern "C" {
    void toggleAlertCallBack(lv_event_t * e) {
      alarmEnabled = lv_obj_has_state(ui_EnableAlarmSwitch, LV_STATE_CHECKED);
       
    }
}

extern "C" {
    void toggleShowDistanceCallBack(lv_event_t * e) {
      showMM = lv_obj_has_state(ui_showDistanceSwitch, LV_STATE_CHECKED);
      initDisplayValues();
    }
}

extern "C" {
    void toggleShowShotCallBack(lv_event_t * e) {
      showShot = lv_obj_has_state(ui_showShotSwitch, LV_STATE_CHECKED);
      initDisplayValues();
    }
}

extern "C" {
    void toggleShowCounterCallBack(lv_event_t * e) {
      showCounter = lv_obj_has_state(ui_showCounterSwitch, LV_STATE_CHECKED);
      initDisplayValues();
    }
}



void setup() {
  String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.begin(115200);
  clearAllPreferences();
  delay(4000);
  initPreferences();
  getPreferences();
  Serial.println(savedsettings.device_name);
  
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

  // Function to draw the GUI (text, buttons and sliders)

  // // Example code snippet
  // const int LEDPIN = 21; // Backlight pin
  // ledcSetup(0, 5000, 8);  // Channel 0, 5kHz, 8-bit res
  // ledcAttachPin(LEDPIN, 0);
  // ledcWrite(0, 200);     // Dim to ~40% brightness (0-255)

  
  ui_init();
  lv_obj_add_flag(ui_resetCounterPanel, LV_OBJ_FLAG_HIDDEN);    // Hide
  initDisplayValues();


}


void updatePowder(int inValue){
  lv_slider_set_value(ui_powderSlider, inValue, LV_ANIM_ON);
  String tempStr = String(inValue) + "%"; 
  const char* myStr = tempStr.c_str(); // Get the underlying const char*
  lv_label_set_text(ui_powderValueLabel, myStr);
  tempStr = String(currentRndCount * 100); 
  myStr = tempStr.c_str(); // Get the underlying const char*
  lv_label_set_text(ui_roundCounterLabel, myStr);
  if (inValue < alertPPercent){
    lv_obj_set_style_bg_color(ui_powderSlider, lv_color_hex(0xFF0000), LV_PART_INDICATOR);
  } else if(inValue < warnPPercent){
    lv_obj_set_style_bg_color(ui_powderSlider, lv_color_hex(0xFF5C00), LV_PART_INDICATOR);
  }
  else {
    lv_obj_set_style_bg_color(ui_powderSlider, lv_color_hex(0x127612), LV_PART_INDICATOR);
  }
  
 
}
int pvalue = 100;
void loop() {
  lv_task_handler();  // let the GUI do its work
  lv_tick_inc(5);     // tell LVGL how much time has passed
  delay(5);           // let this time pass
  lv_timer_handler(); // Update the UI

  static uint32_t last_time = 0;
    if (millis() - last_time > 500) {
        last_time = millis();
      
        pvalue -=1;
        currentRndCount +=1;
        
        updatePowder(pvalue);
        if (pvalue < 1) pvalue = 100;
        if (currentRndCount * 100 > 99999) currentRndCount = 0;
    }
  
}