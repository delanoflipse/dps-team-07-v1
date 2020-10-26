/**
 * @file bluetooth.ino
 * @copyright Delano Flipse 2020
 * Use BLE for proximity detection
 */
 
#include <ArduinoBLE.h>
// Fix for library:
// GAP.ccp#173 HCI.leSetScanParameters(0x01, <interval> + 0x0010, <interval> + 0x0010, 0x00, 0x00

#define BLE_TIME_LIMIT 8000
#define RSSI_LIMIT -85
#define OVERRIDE_BTN A6

// #define USE_OVERRIDE

/* VARIABLES */
boolean forceActive = 0;
int lastOverride = 0;
int maxDelta = 0;
int currentDelta = 0;
unsigned long lastFound = 0;

const int N_DEVICES = 5;
const int rollingAverageBLEReadings = 10;
int deviceRSSIValues [N_DEVICES][rollingAverageBLEReadings];
float deviceValues [N_DEVICES];
unsigned long deviceLastReads [N_DEVICES];

void BLEDiscoveredEvent(BLEDevice device) {
  String deviceName = device.hasLocalName() ? device.localName() : "";

  if (deviceName.startsWith(deviceBaseName) || deviceName.startsWith("Delano")) {
    int deviceRSSI = device.rssi();
    
    unsigned long timeNow = millis();
    int timeDelta = lastFound > 0 ? timeNow - lastFound : -1;
    currentDelta = timeDelta;
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

  // setup reading arrays
  fillArray(deviceValues, N_DEVICES, -100.0f);
  for (int i = 0; i < N_DEVICES; i++) {
    fillArray(deviceRSSIValues[i], rollingAverageBLEReadings, -100);
  }

  // start BLE advertising
  BLE.setDeviceName(deviceName.c_str());
  BLE.setLocalName(deviceName.c_str());
  BLE.setAdvertisingInterval(180);
  BLE.setConnectable(false);
  BLE.advertise();
  BLE.scan(true);
  BLE.setEventHandler(BLEDiscovered, BLEDiscoveredEvent);

  #ifdef USE_OVERRIDE
  // setup override button
  pinMode(OVERRIDE_BTN, INPUT);
  #endif
}

/* get the number of nearby devices and their distance */
void getNearbyDevices() {  
  BLE.poll();
  
  #ifdef USE_OVERRIDE
  int overrideValue = digitalRead(OVERRIDE_BTN);
  
  if (overrideValue == HIGH && lastOverride != overrideValue) {
    forceActive = !forceActive;
  }

  lastOverride = overrideValue;
  #endif
  
  unsigned long timeNow = millis();

//  Serial.print(currentDelta);
//  Serial.print("\t");
//  Serial.print(maxDelta);
//  Serial.println();
 
  if (forceActive) {
    devicesNearby = true;
    numberOfDevicesNearby = 1;
    closestDevice = -100;
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
