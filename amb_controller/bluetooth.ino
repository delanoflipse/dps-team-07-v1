
#include <ArduinoBLE.h>

#define BLE_SCAN_RETRIES 15
#define RSSI_LIMIT -90

// device details
String deviceBaseName = "Ambichair-iot-";
String deviceUniqueName = "1";
String deviceName = deviceBaseName + deviceUniqueName;

// math constants
const float N_CONST = 2;
const float MP_CONST = -65;

/* VARIABLES */
int foundNearbyDevices = 0;
int successfullScans = 0;

unsigned long lastBLEscan = 0;
unsigned long BLEscanInterval = 500;

/* determine if the other device is like us */
boolean isOtherDevice(String peripheralName) {
  // for debugging purposes, detect my phone as well
  if (peripheralName == "Delano 1+6") {
    return true;
  }

  // AMB's share a common name, so detect them on this.
  return peripheralName.startsWith(deviceBaseName);
}

/* check peripheral if they are another AMB */
void checkPeripheral(BLEDevice peripheral) {
  // determine a name
  String p_name = peripheral.hasLocalName()
    ? peripheral.localName()
    : peripheral.deviceName();

  // ignore other BLE devices
  if (!isOtherDevice(p_name)) {
    return;
  }

  // get more data
  int p_rssi = peripheral.rssi();
  String p_adress = peripheral.address();

  // limit to a certain signal strength
  if (p_rssi > RSSI_LIMIT) {
    foundNearbyDevices++;
    Serial.print("Log: Device ");
    Serial.print(p_adress + "/" + p_name);
    Serial.print(" signal strength: ");
    Serial.println(p_rssi);
  }
}

/* get the number of nearby devices and their distance */
void getNearbyDevices() {
  // timed interval for checking nearby devices
  unsigned long timeNow = millis();
  if (timeNow > (lastBLEscan + BLEscanInterval)) {
    // reset counter
    foundNearbyDevices = 0;

    // scan for devices
    BLE.scan();
    BLEDevice nextPeripheral = BLE.available();

    // check if there are devices available
    while(nextPeripheral) {
      checkPeripheral(nextPeripheral);
      nextPeripheral = BLE.available();
    }

    // set timing
    lastBLEscan = timeNow;

    if (foundNearbyDevices > 0) {
      successfullScans = BLE_SCAN_RETRIES;
    } else {
      successfullScans = max(0, successfullScans - 1);
    }

    devicesNearby = successfullScans > 0;
    
//    Serial.print("scan: ");
//    Serial.print(successfullScans);
//    Serial.print("\n");
  }
}

/* get distance based on RSSI value*/
float getDistance(int rssi) {
  return pow(10, (MP_CONST - rssi) / (10 * N_CONST));
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
  BLE.advertise();
}
