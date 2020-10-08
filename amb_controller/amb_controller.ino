/**
 * @file amb_controller.ino
 * @copyright Delano Flipse 2020
 * Control the AMB's behaviour using bluetooth and light sensors
 */

#include <ArduinoBLE.h>

/* CONSTANTS */
#define SENSOR_LIMIT 100
#define SENSOR_CONNECTED

/* VARIABLES */
// state variables
// states: 0 -> dorment, 1 -> picked up, 3-> sit on
int ambState = 0;
int currentVolume = 0;
int targetVolume = 0;

// determination values
boolean sitOn = false;
boolean pickedUp = false;
boolean devicesNearby = false;

/* SETUP */
void setup() {
  // listen on serial connection for messages from the PC
  Serial.begin(9600);

  setupBluetooth();
  setupLights();
  #ifdef SENSOR_CONNECTED
  setupAudio();
  setupPickup();
  setupSitDetector();
  #endif

  // log setup data
  Serial.println("Log: Board setup");
}

void determineStateAndVolume() {

  // determine next state
  switch (ambState) {
    // dorment
    case 0:
      targetVolume = 0;
      // wake up if user is sitting
      if (sitOn) {
        ambState = 2;
      }
      // wake up if user is sitting
      if (pickedUp) {
        ambState = 1;
      }
    break;
    
    // picked up
    case 1:
      targetVolume = 100;
      // wake up if user is sitting
      if (!pickedUp) {
        ambState = 0;
        currentVolume = 0;
      }
    break;
    
    // sit on
    case 2:
      targetVolume = 100;
      if (!sitOn) {
        ambState = 0;
      }

      if (pickedUp) {
        ambState = 1;
      }
    break;
    
    default:
    break;
  }

  // move volume to the target volume
  if (targetVolume != currentVolume) {
    currentVolume += targetVolume > currentVolume ? 1 : -1;
  }
}


/* main control loop */
void loop() {
  // get nearby AMB's
  getNearbyDevices();

  // determine colors, state and volume based on sensor values
  determineStateAndVolume();
  
  #ifdef SENSOR_CONNECTED
  determinePickedUp();
  determineSitOn();
  #endif
  loopAnimations();

  // output
  setLEDs(devicesNearby, ambState);
  
  #ifdef SENSOR_CONNECTED
  setVolume(currentVolume);
  setAudio(devicesNearby, ambState);
  #endif
  
//  Serial.print((int) devicesNearby);
//  Serial.print('\t');
//  Serial.print(ambState);
//  Serial.print('\t');
//  Serial.print(currentVolume);
//  Serial.print('\t');
//  Serial.print((int) pickedUp);
//  Serial.println();
}
