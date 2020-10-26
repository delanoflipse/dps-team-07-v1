/**
 * @file lights.ino
 * @copyright Delano Flipse 2020
 * Animated lights based on the current state
 */
 
/* LIBRARIES */
#include <ChainableLED.h>

/* CONSTANTS */

#define NUM_LEDS 3
ChainableLED leds(7, 8, NUM_LEDS);

// idle fade variables
#define ANIM_1_STATES 4
int animation1[ANIM_1_STATES][3] = {
  // start, end, duration
  {0, 500, 2250},
  {500, 0, 100},
  {0, 500, 100},
  {500, 0, 2305},
};

// idle fade variables
#define ANIM_2_STATES 7
int animation2[ANIM_2_STATES][3] = {
  // start, end, duration
  {0, 500, 2505},
  {500, 500, 2528},
  {500, 300, 2860},
  {300, 500, 2420},
  {500, 0, 100},
  {0, 500, 100},
  {500, 0, 2555},
};

// idle fade variables
#define ANIM_3_STATES 5
int animation3[ANIM_2_STATES][3] = {
  // start, end, duration
  {0, 500, 2500},
  {500, 500, 2500},
  {500, 0, 100},
  {0, 500, 100},
  {500, 0, 2500},
};
// idle fade variables
#define ANIM_ERR_STATES 5
int animation_error[ANIM_ERR_STATES][3] = {
  // start, end, duration
  {0, 500, 500},
  {500, 0, 100},
};

/* VARIABLES */
int animation1_values[] = {0, 0, 0};
int animation2_values[] = {0, 0, 0};
int animation3_values[] = {0, 0, 0};
int animation_error_values[] = {0, 0, 0};

unsigned long lastAnimationUpdate = 0;

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
  progressAnimation(animation_error, ANIM_ERR_STATES, animation_error_values, delta);
}

float getHue(float hue) {
  return hue / 360.0;
}

/* calculate a new color for the LED's */
void setLEDs(boolean devicesNearby, Orientation orientation, MachineState state) {
  float hue1 = 0;
  float hue2 = 0;
  float hue3 = 0;

  float light1 = 0;
  float light2 = 0;
  float light3 = 0;

  float saturation1 = 1.0;
  float saturation2 = 1.0;
  float saturation3 = 1.0;

  switch(orientation) {
    case up:
    // none
    saturation1 = 0.1;
    saturation2 = 0.2;
    saturation3 = 0.1;
    hue1 = getHue(190.0);
    hue2 = getHue(206.0);
    hue3 = getHue(215.0);
    break;
    
    case down:
    // rain
    hue1 = getHue(190.0);
    hue2 = getHue(206.0);
    hue3 = getHue(215.0);
    break;
    
    case right:
    // nature
    hue1 = getHue(78.0);
    hue2 = getHue(96.0);
    hue3 = getHue(115.0);
    break;
    
    case left:
    // campfire
    hue1 = getHue(0.0);
    hue2 = getHue(15.0);
    hue3 = getHue(30.0);
    break;
    
    case front:
    // zen
    saturation1 = 0.5;
    saturation2 = 0.5;
    saturation3 = 0.5;
    hue1 = getHue(300.0);
    hue2 = getHue(265.0);
    hue3 = getHue(240.0);
    break;
    
    case back:
    // music
    hue1 = getHue(34.0);
    hue2 = getHue(46.0);
    hue3 = getHue(55.0);
    break;

    default:
    break;
  }
  
  switch(state) {
    case siton:
    case dorment:
    // keep orientation color, use animations
    light1 = animation1_values[0] / 1024.0;
    light2 = animation2_values[0] / 1024.0;
    light3 = animation3_values[0] / 1024.0;
    break;

    case error:
      // use alarm
      saturation1 = 1.0;
      saturation2 = 1.0;
      saturation3 = 1.0;
      hue1 = getHue(0.0);
      hue2 = getHue(0.0);
      hue3 = getHue(0.0);
      light1 = animation_error_values[0] / 1024.0;
      light2 = animation_error_values[0] / 1024.0;
      light3 = animation_error_values[0] / 1024.0;
    break;
    
    case moving:
      // override colors
      light1 = animation1_values[0] / 1024.0;
      light2 = animation2_values[0] / 1024.0;
      light3 = animation3_values[0] / 1024.0;
    break;
    
    default:
    break;
  }

  // set LED's
  leds.setColorHSB(0, hue1, saturation1, light1);
  leds.setColorHSB(1, hue2, saturation2, light2);
  leds.setColorHSB(2, hue3, saturation3, light3);
}
