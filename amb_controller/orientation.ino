/**
 * @file orientation.ino
 * @copyright Delano Flipse 2020
 * Determine orientation based on gravity working on the accelerometers.
 */
 
#include <Arduino_LSM6DS3.h>

#define GRAVITY 0.98
#define ORIENTATION_ACC_MARGIN 0.2
#define ORIENTATION_DRIFT_MARGIN 0.3
#define ORIENTATION_GRAV_MARGIN 0.9

// the setup function runs once when you press reset or power the board
void setupOrientation() {
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }
}

// the loop function runs over and over again forever
void determineOrientation() { 
  float x, y, z, acc;
  int moving = 1;

  if (!forceOrientation && IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    acc = x * x + y * y + z * z;
    boolean smallX = abs(x) < ORIENTATION_DRIFT_MARGIN;
    boolean smallY = abs(y) < ORIENTATION_DRIFT_MARGIN;
    boolean smallZ = abs(z) < ORIENTATION_DRIFT_MARGIN;
    boolean gravX = abs(x) > ORIENTATION_GRAV_MARGIN;
    boolean gravY = abs(y) > ORIENTATION_GRAV_MARGIN;
    boolean gravZ = abs(z) > ORIENTATION_GRAV_MARGIN;

    if (acc < GRAVITY + ORIENTATION_ACC_MARGIN && acc > GRAVITY - ORIENTATION_ACC_MARGIN) {
      moving = 0;

      if (smallX && smallY && gravZ && z > 0) {
        currentOrientation = up;
      } else if (smallX && smallY && gravZ && z < 0) {
        currentOrientation = down;
      } else if (smallX && smallZ && gravY && y > 0) {
        currentOrientation = left;
      } else if (smallX && smallZ && gravY && y < 0) {
        currentOrientation = right;
      } else if (smallZ && smallY && gravX && x > 0) {
        currentOrientation = back;
      } else if (smallZ && smallY && gravX && x < 0) {
        currentOrientation = front;
      } else {
        // ?
      }
    }
  }
}
