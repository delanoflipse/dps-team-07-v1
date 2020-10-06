
#define Z_OFFSET 0.98
#define Z_TRIGGER 0.12
#define Z_MAX_NOISE 0.03

#define IR_SENSOR_PIN A2
#define IR_SENSOR_UPPER_LIMIT 400
#define IR_SENSOR_LOWER_LIMIT 350

//#define USE_ACCELEROMETER


const int numReadings = 10;
float readingsZ[numReadings];

void createEmptyArray(float arr[], const int len) {
  for (int i = 0; i < len; i++) {
    arr[i] = 0.0;
  }
}

float runningTotal(float arr[], const int len, float newValue) {
  float total = newValue;
  for (int i = 1; i < len; i++) {
    total += arr[i];
    arr[i-1] = arr[i];
  }

  arr[len - 1] = newValue;
  return total / len;
}

#ifdef USE_ACCELEROMETER
#include <Arduino_LSM6DS3.h>

int calibrationReadingsLeft = numReadings * 10;

int moveState = 0;
float initialZ = 0;

// the setup function runs once when you press reset or power the board
void setupPickup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }
 
  createEmptyArray(readingsZ, numReadings);
}

// the loop function runs over and over again forever
void determinePickedUp() { 
  float x, y, z;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    z -= Z_OFFSET;
    float runningAverageZ = runningTotal(readingsZ, numReadings, z) - initialZ;


    // calibrate
    if (calibrationReadingsLeft > 0) {
      calibrationReadingsLeft--;
      return;
    } else if (calibrationReadingsLeft == 0) {
      calibrationReadingsLeft--;
      initialZ = runningAverageZ;
      digitalWrite(LED_BUILTIN, LOW);
      return;
    }
    
//    Serial.print(runningAverageZ);
//    Serial.print("\t");
//    Serial.print(moveState);
//    Serial.println();

    switch (moveState) {
      case 0:
        pickedUp = false;
        // LAYING
        if (runningAverageZ > Z_TRIGGER) {
          moveState++;
        }
        if (runningAverageZ < -Z_TRIGGER) {
          moveState = 4;
        }
      break;
      
      case 1:
        pickedUp = true;
        // Moving up
        if (runningAverageZ < -Z_TRIGGER) {
          moveState++;
        }
      break;
      
      case 2:
        pickedUp = true;
        // stopped up
        if (runningAverageZ > -Z_MAX_NOISE) {
          moveState++;
        }
      break;
      
      case 3:
        pickedUp = true;
        // moving down
        if (runningAverageZ < -Z_TRIGGER) {
          moveState++;
        }
        if (runningAverageZ > Z_TRIGGER) {
          moveState = 1;
        }
      break;
      
      case 4:
        pickedUp = true;
        // stopped moving down
        if (runningAverageZ > Z_TRIGGER) {
          moveState++;
        }
      break;
      
      case 5:
        pickedUp = true;
        // stopped moving down
        if (runningAverageZ < Z_MAX_NOISE) {
          moveState = 0;
        }
      break;
    }
  }
}
#else
void setupPickup() {  
  // setup pins
  pinMode(IR_SENSOR_PIN, INPUT);
  createEmptyArray(readingsZ, numReadings);
}

void determinePickedUp() {
  int sensor = analogRead(IR_SENSOR_PIN);
  float runningAverageZ = runningTotal(readingsZ, numReadings, sensor);

  if (!pickedUp && runningAverageZ < IR_SENSOR_LOWER_LIMIT) {
    pickedUp = true;
  }
  
  if (pickedUp && runningAverageZ > IR_SENSOR_UPPER_LIMIT) {
    pickedUp = false;
  }
    
  Serial.print(runningAverageZ);
  Serial.print("\t");
  Serial.print((int) pickedUp);
  Serial.println();
}
#endif
