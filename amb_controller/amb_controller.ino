/**
 * @file amb_controller.ino
 * @copyright Delano Flipse 2020
 * Control the AMB's behaviour using bluetooth and light sensors
 */

#include <ArduinoBLE.h>
#include "arduino_secrets.h"

// device details
String deviceBaseName = "Ambichair-iot-";
String deviceUniqueName = "1";
String deviceName = deviceBaseName + deviceUniqueName;

String lastLog;
unsigned long lastLogTime;

/* CONSTANTS */
#define USE_SENSORS
#define USE_AUDIO
#define USE_BLE_PROXIMITY
#define USE_LIGHTS
#define USE_ORIENTATION
//#define USE_WIFI

#define RSSI_UPPER_LIMIT -60
#define RSSI_LOWER_LIMIT -65

enum Orientation { left, right, front, back, up, down, none };
Orientation currentOrientation = up;
enum MachineState { error, dorment, moving, siton };
MachineState currentState = dorment;

/* VARIABLES */
// state variables
int currentVolume = 0;
int targetVolume = 0;

// determination values
boolean sitOn = false;
boolean pickedUp = false;
boolean devicesNearby = false;
boolean forceState = false;
int numberOfDevicesNearby = 0;
int closestDevice = -100;

/* SETUP */
void setup() {
  // listen on serial connection for messages from the PC
  Serial.begin(9600);

  #ifdef USE_BLE_PROXIMITY
  setupBluetooth();
  #endif
  
  #ifdef USE_WIFI
  setupWiFi();
  #endif
  
  #ifdef USE_LIGHTS
  setupLights();
  #endif
  
  #ifdef USE_ORIENTATION
  setupOrientation();
  #endif
  
  #ifdef USE_AUDIO
  setupAudio();
  #endif
  
  #ifdef USE_SENSORS
//  setupPickup();
  setupSitDetector();
  #endif

  lastLogTime = millis();
  // log setup data
  Serial.println("Log: Board setup");
}

void determineStateAndVolume() {

  // determine next state
  switch (currentState) {
    case dorment:
      targetVolume = 0;

      if (!forceState) {
        if (sitOn) {
          currentState = siton;
        }
  
        if (pickedUp) {
          currentState = moving;
        }
        
        if (closestDevice > RSSI_UPPER_LIMIT ) {
          currentState = error;
        }
      }
    break;
    
    // picked up
    case moving:
      targetVolume = 31;
      
      if (!forceState) {
        if (!pickedUp) {
          currentState = dorment;
          currentVolume = 0;
        }
      }
    break;

    case error:
      targetVolume = 31;
      
      if (!forceState) {
        if (closestDevice < RSSI_LOWER_LIMIT ) {
          currentState = dorment;
        }
      }
    break;
    
    // sit on
    case siton:
      targetVolume = 31;
      
      if (!forceState) {
        if (!sitOn) {
          currentState = dorment;
        }
  
        if (pickedUp) {
          currentState = moving;
        }
        
        if (closestDevice > RSSI_UPPER_LIMIT ) {
          currentState = error;
        }
      }
    break;
    
    default:
    currentState = currentState;
    break;
  }

  // move volume to the target volume
  if (targetVolume != currentVolume) {
    currentVolume += targetVolume > currentVolume ? 1 : -1;
  }
}


/* main control loop */
void loop() {
  // input
  #ifdef USE_BLE_PROXIMITY
  // get nearby AMB's
  getNearbyDevices();
  #endif
  
  #ifdef USE_WIFI
  loopWiFi();
  #endif
  
  #ifdef USE_SENSORS
//  determinePickedUp();
  determineSitOn();
  #endif
  
  #ifdef USE_ORIENTATION
  determineOrientation();
  #endif

  // determine colors, state and volume based on sensor values
  determineStateAndVolume();

  // output  
  #ifdef USE_LIGHTS
  loopAnimations();
  setLEDs(devicesNearby, currentOrientation, currentState);
  #endif
  
  #ifdef USE_AUDIO
  setAudio(devicesNearby, currentOrientation, currentState);
  setVolume(currentVolume);
  #endif

  unsigned long timing = millis();
  String logStr = deviceName
    + '\t' + numberOfDevicesNearby
    + '\t' + closestDevice
    + '\t' + stateToString(currentState)
    + '\t' + orientationToString(currentOrientation)
    + '\t' + (int) forceState
    + '\t' + pickedUp;
      
  if (lastLog != logStr || timing > lastLogTime + 1000) {
    lastLog = logStr;
    lastLogTime = timing;
    
    // LOG EVENTS
    #ifdef USE_WIFI
    logWiFi(logStr);
    #else
    Serial.println(logStr);
    #endif
  }

}
