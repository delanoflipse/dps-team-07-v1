/**
 * @file amb_controller.ino
 * @copyright Delano Flipse 2020
 * Control the AMB's behaviour using bluetooth and light sensors
 */

#include <ArduinoBLE.h>

/* CONSTANTS */
#define SENSOR_LIMIT 100
//#define USE_SENSORS
//#define USE_AUDIO
#define USE_BLE_PROXIMITY
#define USE_LIGHTS
#define USE_ORIENTATION

#define RSSI_UPPER_LIMIT -60
#define RSSI_LOWER_LIMIT -65

enum Orientation { left, right, front, back, up, down, none };
Orientation currentOrientation = up;
enum MachineState { error, dorment, moving, siton };
MachineState ambState = dorment;

/* VARIABLES */
// state variables
int currentVolume = 0;
int targetVolume = 0;

// determination values
boolean sitOn = false;
boolean pickedUp = false;
boolean devicesNearby = false;
int numberOfDevicesNearby = 0;
int closestDevice = -100;

/* SETUP */
void setup() {
  // listen on serial connection for messages from the PC
  Serial.begin(9600);

  #ifdef USE_BLE_PROXIMITY
  setupBluetooth();
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

  // log setup data
  Serial.println("Log: Board setup");
}

void determineStateAndVolume() {

  // determine next state
  switch (ambState) {
    case dorment:
      targetVolume = 0;
      // wake up if user is sitting
      if (sitOn) {
        ambState = siton;
      }
      // wake up if user is sitting
      if (pickedUp) {
        ambState = moving;
      }
      
      if (closestDevice > RSSI_UPPER_LIMIT ) {
        ambState = error;
      }
    break;
    
    // picked up
    case moving:
      targetVolume = 31;
      // wake up if user is sitting
      if (!pickedUp) {
        ambState = dorment;
        currentVolume = 0;
      }
    break;

    case error:
      if (closestDevice < RSSI_LOWER_LIMIT ) {
        ambState = dorment;
      }
    break;
    
    // sit on
    case siton:
      targetVolume = 31;
      if (!sitOn) {
        ambState = dorment;
      }

      if (pickedUp) {
        ambState = moving;
      }
    break;
    
    default:
    ambState = ambState;
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
  setLEDs(devicesNearby, currentOrientation, ambState);
  #endif
  
  #ifdef USE_AUDIO
  setVolume(currentVolume);
  setAudio(devicesNearby, ambState);
  #endif
  
  Serial.print(numberOfDevicesNearby);
  Serial.print('\t');
  Serial.print(closestDevice);
  Serial.print('\t');  
  Serial.print(stateToString(ambState));
  Serial.print('\t');
  Serial.print(orientationToString(currentOrientation));
  Serial.print('\t');
  Serial.print((int) pickedUp);
  Serial.println();
}
