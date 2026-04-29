struct settings {
  String device_name;  // 1
  String device_desc;  // 2
  String mdns_name;
  bool alarmEnabled;
  int minPDist;  // The closest reading to the sensor in mm: 100% full
  int maxPDist; // The furthest reading from the sensor in mm: 0% full
  int minSDist;  // The closest reading to the sensor in mm: 100% full
  int maxSDist; // The furthest reading from the sensor in mm: 0% full
  int alertPPercent;
  int alertSPercent;
  int currentRndCount;
  int totalRndCnt;
  bool showMM;
  bool showShot;
  bool showCounter;
 };

extern settings savedsettings;
void clearAllPreferences();
void initPreferences();
void storePreferences();
void getPreferences();