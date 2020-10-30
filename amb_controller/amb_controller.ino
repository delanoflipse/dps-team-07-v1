/**
 * @file amb_controller.ino
 * @copyright Delano Flipse 2020
 * Controls the overall AMB's behaviour
 */

#include <ArduinoBLE.h>
#include "arduino_secrets.h"

// device details
String deviceBaseName = "AMB-IOT-";
String deviceUniqueName = "2";
String deviceName = deviceBaseName + deviceUniqueName;

String lastLog;
unsigned long lastLogTime;

unsigned long now;

/* CONSTANTS */
//#define USE_SENSORS
#define USE_AUDIO
#define USE_LIGHTS
#define USE_ORIENTATION
#define USE_WIFI

#define CLOSEST_DEVICE_ERROR_MIN_DISTANCE 1400
#define CLOSEST_DEVICE_GROUP_MIN_DISTANCE 3000
#define ALARM_REPLAYS 4

bool forceOrientation = false;
enum Orientation { left, right, front, back, up, down, none };
Orientation currentOrientation = up;
Orientation closestOrientation = up;

enum Proximity { close, group, alone };
Proximity currentProximity = alone;
enum MachineState { error, dorment, quiet, active };
MachineState currentState = active;
MachineState lastState = currentState;

/* VARIABLES */
int currentVolume = 0;
int targetVolume = 0;
int maxVolume = 12;
int minVolume = 8;
int tapValue = 0;

// determination values
int closestDevice = 8000;
unsigned long lastStateChange;

/* SETUP */
void setup() {
  // listen on serial connection for messages from the PC
  Serial.begin(9600);
  now = millis();
  lastLogTime = now;
  lastStateChange = now;
  
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
  setupTap();
  #endif

  // log setup data
  Serial.println("Log: Board setup");
}

void determineStateAndVolume() {

  if (closestDevice < CLOSEST_DEVICE_ERROR_MIN_DISTANCE) {
    currentProximity = close;
  } else if (closestDevice < CLOSEST_DEVICE_GROUP_MIN_DISTANCE) {
    currentProximity = group;
  } else {
    currentProximity = alone;
  }

  // determine next state
  switch (currentState) {
    case dorment:
      if (currentProximity == close) {
        currentState = error;
      }
    break;

    case error:
      if (currentProximity != close) {
        currentState = active;
      }
    break;

    
    case quiet:
      if (currentProximity == close) {
        currentState = error;
      }
      if (currentProximity == alone) {
        currentState = active;
      }
    break;
    
    case active:
      if (currentProximity == close) {
        currentState = error;
      }
    break;
    
    default:
      currentState = currentState;
    break;
  }
}


/* main control loop */
void loop() {
  now = millis();

  // input  
  #ifdef USE_WIFI
  loopWiFi();
  #endif
  
  #ifdef USE_SENSORS
  determineTap();
  #endif
  
  #ifdef USE_ORIENTATION
  determineOrientation();
  #endif

  // determine colors, state and volume based on sensor values
  determineStateAndVolume();
  

  // output  
  #ifdef USE_LIGHTS
  setLEDs(currentOrientation, currentProximity, currentState, lastState);
  #endif
  
  #ifdef USE_AUDIO
  setAudio(currentOrientation, currentProximity, currentState, lastState);
  #endif

  String logStr = deviceName
    + '\t' + closestDevice
    + '\t' + stateToString(currentState)
    + '\t' + orientationToString(currentOrientation)
    + '\t' + proximityToString(currentProximity)
    + '\t' + (int) forceOrientation;
      
  if (lastLog != logStr || now > lastLogTime + 1000) {
    lastLog = logStr;
    lastLogTime = now;
    
    // LOG EVENTS
    #ifdef USE_WIFI
    logWiFi(logStr);
    #else
//    Serial.println(logStr);
    #endif
  }

  if (lastState != currentState) {
      lastStateChange = now;
  }

  lastState = currentState;
}
