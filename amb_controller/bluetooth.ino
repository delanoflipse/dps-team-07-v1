
#include <ArduinoBLE.h>
// Fix for library:
// GAP.ccp#173 HCI.leSetScanParameters(0x01, 180 + 0x0010, 180 + 0x0010, 0x00, 0x00

#define BLE_TIME_LIMIT 8000
#define RSSI_LIMIT -85
#define OVERRIDE_BTN A6

// #define USE_OVERRIDE

// device details
String deviceBaseName = "Ambichair-iot-";
String deviceUniqueName = "1";
String deviceName = deviceBaseName + deviceUniqueName;

// math constants
const float N_CONST = 2;
const float MP_CONST = -65;

/* VARIABLES */
boolean forceActive = 0;
int lastOverride = 0;
int maxDelta = 0;
unsigned long lastFound = 0;

const int N_DEVICES = 5;
const int rollingAverageBLEReadings = 10;
int deviceRSSIValues [N_DEVICES][rollingAverageBLEReadings];
float deviceValues [N_DEVICES];
unsigned long deviceLastReads [N_DEVICES];

void BLEDiscoveredEvent(BLEDevice device) {
  String deviceName = device.hasLocalName() ? device.localName() : "";

  if (deviceName.startsWith(deviceBaseName)) {
    int deviceRSSI = device.rssi();
    
    unsigned long timeNow = millis();
    int timeDelta = lastFound > 0 ? timeNow - lastFound : -1;
    lastFound = timeNow;
    
    String deviceIndex = deviceName.substring(deviceBaseName.length(), deviceName.length());
    int index = deviceIndex.toInt();

    deviceValues[index] = runningMaximum(deviceRSSIValues[index], rollingAverageBLEReadings, deviceRSSI);
    deviceLastReads[index] = lastFound;
   
    maxDelta = max(maxDelta, timeDelta);
  }
}

/* init bluetooth and start advertising */
void setupBluetooth() {
  // begin Bluetooth LE initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // start BLE advertising
  BLE.setDeviceName(deviceName.c_str());
  BLE.setLocalName(deviceName.c_str());
  BLE.setAdvertisingInterval(180);
  BLE.setConnectable(false);
  BLE.advertise();
  BLE.scan(true);
  BLE.setEventHandler(BLEDiscovered, BLEDiscoveredEvent);

  fillArray(deviceValues, N_DEVICES, -100.0f);
  for (int i = 0; i < N_DEVICES; i++) {
    fillArray(deviceRSSIValues[i], rollingAverageBLEReadings, -100);
  }

  #ifdef USE_OVERRIDE
  pinMode(OVERRIDE_BTN, INPUT);
  #endif
}

/* get the number of nearby devices and their distance */
void getNearbyDevices() {  
  #ifdef USE_OVERRIDE
  int overrideValue = digitalRead(OVERRIDE_BTN);
  
  if (overrideValue == HIGH && lastOverride != overrideValue) {
    forceActive = !forceActive;
  }

  lastOverride = overrideValue;
  #endif
  
  // timed interval for checking nearby devices
  unsigned long timeNow = millis();
  BLE.poll();

  Serial.println(maxDelta);
 
  if (forceActive) {
    devicesNearby = true;
  } else {
    devicesNearby = false;
    numberOfDevicesNearby = 0;
    closestDevice = -100;
    
    for (int i = 0; i < N_DEVICES; i++) {
      unsigned long lastFound = deviceLastReads[i];
      float deviceRSSI = deviceValues[i];
      bool foundDevice = lastFound > 0;
      
      if (foundDevice && timeNow < lastFound + BLE_TIME_LIMIT) {
        devicesNearby = true;
        numberOfDevicesNearby++;
        closestDevice = max(closestDevice, (int) deviceRSSI);
      }
    }
  }
}

/* get distance based on RSSI value*/
float getDistance(int rssi) {
  return pow(10, (MP_CONST - rssi) / (10 * N_CONST));
}
