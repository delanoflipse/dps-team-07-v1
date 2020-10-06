
/* LIBRARIES */
#include <ChainableLED.h>

/* CONSTANTS */

#define NUM_LEDS 3

/* VARIABLES */

// idle fade variables
#define ANIM_1_STATES 2
int animation1[ANIM_1_STATES][3] = {
  // start, end, duration
  {0, 500, 2250},
  {500, 0, 2305},
};

// idle fade variables
#define ANIM_2_STATES 5
int animation2[ANIM_2_STATES][3] = {
  // start, end, duration
  {0, 500, 2505},
  {500, 500, 2528},
  {500, 300, 2860},
  {300, 500, 2420},
  {500, 0, 2555},
};

// idle fade variables
#define ANIM_3_STATES 5
int animation3[ANIM_2_STATES][3] = {
  // start, end, duration
  {0, 500, 2500},
  {500, 500, 2500},
  {500, 0, 2500},
};

int animation1_values[] = {0, 0, 0};
int animation2_values[] = {0, 0, 0};
int animation3_values[] = {0, 0, 0};

unsigned long lastAnimationUpdate = 0;

ChainableLED leds(7, 8, NUM_LEDS);

void setupLights() {
  // intial timing
  unsigned long timeNow = millis();
  lastAnimationUpdate = timeNow;
}

void progressAnimation(int animation[][3], int arrayLength, int values[3], int delta) {
  int *index = &values[2];
  int *progress = &values[1];
  
  int start = animation[*index][0];  
  int end = animation[*index][1];
  int duration = animation[*index][2];
  *progress += delta;
  
  if (*progress > duration) {
    *progress = *progress - duration;
    if (*index + 1 < arrayLength) {
      *index += 1; 
    } else {
      *index = 0;
    }

    progressAnimation(animation, arrayLength, values, 0);
    return;
  }

  values[0] = map(*progress, 0, duration, start, end);
}

void loopAnimations() {
  unsigned long timeNow = millis();
  int delta = timeNow - lastAnimationUpdate;
  lastAnimationUpdate = timeNow;
  
  progressAnimation(animation1, ANIM_1_STATES, animation1_values, delta);
  progressAnimation(animation2, ANIM_2_STATES, animation2_values, delta);
  progressAnimation(animation3, ANIM_3_STATES, animation3_values, delta);
}

/* calculate a new color for the LED's */
void setLEDs(boolean devicesNearby, int state) {
  // active = red/yellow
  // dorment = greenish
  float hue1 = devicesNearby ? 0 : 125.0 / 360.0;
  float hue2 = devicesNearby ? 15.0 / 360.0 : 120.0 / 360.0;
  float hue3 = devicesNearby ? 30.0 / 360.0 : 115.0 / 360.0;

  float saturation = 1.0;

  float light1 = 0;
  float light2 = 0;
  float light3 = 0;

  
  switch (state) {
    // dorment
    case 0:
      light1 = animation1_values[0] / 1024.0;
      light2 = animation2_values[0] / 1024.0;
      light3 = animation3_values[0] / 1024.0;
    break;
    
    case 1:
      hue1 = 285.0 / 360.0;
      hue2 = 270.0 / 360.0;
      hue3 = 255.0 / 360.0;
      light1 = animation1_values[0] / 1024.0;
      light2 = animation2_values[0] / 1024.0;
      light3 = animation3_values[0] / 1024.0;
    // picked up
    break;
    
    // sit on
    case 2:
      light1 = animation1_values[0] / 1024.0;
      light2 = animation2_values[0] / 1024.0;
      light3 = animation3_values[0] / 1024.0;
    break;
    
    default:
    break;
  }

  // brightness is based on state
  // active - based on music samples
  // passive - based on fade loop
  
  leds.setColorHSB(0, hue1, saturation, light1);
  leds.setColorHSB(1, hue2, saturation, light2);
  leds.setColorHSB(2, hue3, saturation, light3);
}
