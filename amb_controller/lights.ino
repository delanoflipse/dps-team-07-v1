/**
 * @file lights.ino
 * @copyright Delano Flipse 2020
 * Animated lights based on the current state
 */
 
/* LIBRARIES */
#include <ChainableLED.h>

/* CONSTANTS */

struct HSL {
  float hue;
  float saturation;
  float lightness;
};

struct HSL lights[6] = {
  { 0.0, 0.0, 0.0 },
  { 0.0, 0.0, 0.0 },
  { 0.0, 0.0, 0.0 },
  { 0.0, 0.0, 0.0 },
  { 0.0, 0.0, 0.0 },
  { 0.0, 0.0, 0.0 },
};

#define NUM_LEDS 6
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
#define ANIM_ERR_STATES 4
int animation_error[ANIM_ERR_STATES][3] = {
  // start, end, duration
  {0, 0, 50},
  {0, 500, 500},
  {500, 0, 100},
  {0, 0, 1750},
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

void loopAnimations(Orientation orientation, Proximity proximity, MachineState state, MachineState lastState) {
  int delta = now - lastAnimationUpdate;
  lastAnimationUpdate = now;
  
  progressAnimation(animation1, ANIM_1_STATES, animation1_values, delta);
  progressAnimation(animation2, ANIM_2_STATES, animation2_values, delta);
  progressAnimation(animation3, ANIM_3_STATES, animation3_values, delta);
  
  if (state == error && lastState != error) {
    animation_error_values[0] = 0;
    animation_error_values[1] = 0;
    animation_error_values[2] = 0;
  }
  
  progressAnimation(animation_error, ANIM_ERR_STATES, animation_error_values, delta);
}

float getHue(float hue) {
  return hue / 360.0;
}

void setLed(int ledIndex, int lightIndex) {
  Serial.print(ledIndex);
  Serial.print(' ');
  Serial.println(HSLtoString(lights[lightIndex]));
  leds.setColorHSB(ledIndex, lights[lightIndex].hue, lights[lightIndex].saturation, lights[lightIndex].lightness);
}

void getLightsForOrientation(Orientation orientation, int startIndex) {
  lights[startIndex].saturation = 1.0;
  lights[startIndex + 1].saturation = 1.0;
  lights[startIndex + 2].saturation = 1.0;
  switch(orientation) {
    case up:
    // nature
    lights[startIndex].hue = getHue(78.0);
    lights[startIndex + 1].hue = getHue(96.0);
    lights[startIndex + 2].hue = getHue(115.0);
    break;
    
    case down:
    // none
    lights[startIndex].saturation = 0.1;
    lights[startIndex + 1].saturation = 0.2;
    lights[startIndex + 2].saturation = 0.1;
    
    lights[startIndex].hue = getHue(190.0);
    lights[startIndex + 1].hue = getHue(206.0);
    lights[startIndex + 2].hue = getHue(215.0);
    break;
    
    case right:
    // rain
    lights[startIndex].hue = getHue(190.0);
    lights[startIndex + 1].hue = getHue(206.0);
    lights[startIndex + 2].hue = getHue(215.0);
    break;
    
    case left:
    // campfire
    lights[startIndex].hue = getHue(0.0);
    lights[startIndex + 1].hue = getHue(15.0);
    lights[startIndex + 2].hue = getHue(30.0);
    break;
    
    case front:
    // music
    lights[startIndex].hue = getHue(34.0);
    lights[startIndex + 1].hue = getHue(46.0);
    lights[startIndex + 2].hue = getHue(55.0);
    break;
    
    case back:
    // zen
    lights[startIndex].saturation = 0.5;
    lights[startIndex + 1].saturation = 0.5;
    lights[startIndex + 2].saturation = 0.5;
    lights[startIndex].hue = getHue(300.0);
    lights[startIndex + 1].hue = getHue(265.0);
    lights[startIndex + 2].hue = getHue(240.0);
    break;

    default:
    break;
  }
}

/* calculate a new color for the LED's */
void setLEDs(Orientation orientation, Proximity proximity, MachineState state, MachineState lastState) {
  loopAnimations(orientation, proximity, state, lastState);

  getLightsForOrientation(orientation, 0);

  if (proximity == group) {
    getLightsForOrientation(closestOrientation, 3);
  } else {
    getLightsForOrientation(orientation, 3);
  }
  
  switch(state) {
    case active:
    case dorment:
    // keep orientation color, use animations
    lights[0].lightness = animation1_values[0] / 1024.0 + tapValue / 1024.0;
    lights[1].lightness = animation2_values[0] / 1024.0 + tapValue / 1024.0;
    lights[2].lightness = animation3_values[0] / 1024.0 + tapValue / 1024.0;
    lights[3].lightness = animation1_values[0] / 1024.0 + tapValue / 1024.0;
    lights[4].lightness = animation2_values[0] / 1024.0 + tapValue / 1024.0;
    lights[5].lightness = animation3_values[0] / 1024.0 + tapValue / 1024.0;
    
    setLed(0, 0);
    setLed(1, 1);
    setLed(2, 2);
    setLed(3, 4);
    setLed(4, 5);
    setLed(5, 1);
    break;

    case error:
      // alarm      
      if (lastStateChange + ALARM_REPLAYS * 2400 > now) {
        lights[0].hue = getHue(0.0);
        lights[1].hue = getHue(0.0);
        lights[2].hue = getHue(0.0);
        lights[3].hue = getHue(0.0);
        lights[4].hue = getHue(0.0);
        lights[5].hue = getHue(0.0);
        lights[0].saturation = 1.0;
        lights[1].saturation = 1.0;
        lights[2].saturation = 1.0;
        lights[3].saturation = 1.0;
        lights[4].saturation = 1.0;
        lights[5].saturation = 1.0;
      } else {
        lights[0].hue = getHue(0.0);
        lights[1].hue = getHue(0.0);
        lights[2].hue = getHue(0.0);
        lights[0].saturation = 1.0;
        lights[1].saturation = 1.0;
        lights[2].saturation = 1.0;
      }
      lights[0].lightness = animation_error_values[0] / 1024.0;
      lights[1].lightness = animation_error_values[0] / 1024.0;
      lights[2].lightness = animation_error_values[0] / 1024.0;
      lights[3].lightness = animation_error_values[0] / 1024.0;
      lights[4].lightness = animation_error_values[0] / 1024.0;
      lights[5].lightness = animation_error_values[0] / 1024.0;
      
      setLed(0, 0);
      setLed(1, 1);
      setLed(2, 2);
      setLed(3, 4);
      setLed(4, 5);
      setLed(5, 1);
    break;
    
    default:
    break;
  }
}
