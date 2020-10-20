
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

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    acc = x * x + y * y + z * z;
    boolean smallX = abs(x) < ORIENTATION_DRIFT_MARGIN;
    boolean smallY = abs(y) < ORIENTATION_DRIFT_MARGIN;
    boolean smallZ = abs(z) < ORIENTATION_DRIFT_MARGIN;
    boolean gravX = abs(x) > ORIENTATION_GRAV_MARGIN;
    boolean gravY = abs(y) > ORIENTATION_GRAV_MARGIN;
    boolean gravZ = abs(z) > ORIENTATION_GRAV_MARGIN;
    
    switch (currentOrientation) {
      case none:
      break;
      default:
      break;
    }
  
//    Serial.print(x);
//    Serial.print('\t');
//    Serial.print(y);
//    Serial.print('\t');
//    Serial.print(z);
//    Serial.print('\t');
//    Serial.print(acc);
//    Serial.print('\t');
//    Serial.print(moving);

    if (acc < GRAVITY + ORIENTATION_ACC_MARGIN && acc > GRAVITY - ORIENTATION_ACC_MARGIN) {
      moving = 0;
      
//      Serial.print(0);
//      Serial.print('\t');

      if (smallX && smallY && gravZ && z > 0) {
        currentOrientation = up;
//        Serial.print("up");
      } else if (smallX && smallY && gravZ && z < 0) {
        currentOrientation = down;
//        Serial.print("down");
      } else if (smallX && smallZ && gravY && y > 0) {
        currentOrientation = left;
//        Serial.print("left");
      } else if (smallX && smallZ && gravY && y < 0) {
        currentOrientation = right;
//        Serial.print("right");
      } else if (smallZ && smallY && gravX && x > 0) {
        currentOrientation = back;
//        Serial.print("back");
      } else if (smallZ && smallY && gravX && x < 0) {
        currentOrientation = front;
//        Serial.print("front");
      } else {
//        currentOrientation = none;
//        Serial.print("none");
      }
    } else {
//      Serial.print(1);
//      Serial.print('\t');
//      Serial.print(-1);
    }
//    
//    Serial.println();
  }
}
