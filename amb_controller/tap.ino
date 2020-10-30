/**
 * @file sit_on.ino
 * @copyright Delano Flipse 2020
 * ! Work in progress
 * A toggle for user presence.
 */
 
#define SIT_DETECTOR_PIN A0

#define TAP_LOWER_LIMIT 140
#define TAP_UPPER_LIMIT 145
#define TAP_EVENT_DURATION 100

int tapState = 0;
int lastTapState = 0;

unsigned long tapEventStart;
int tapEventState = 0;

void setupTap() {
  // setup pins
  pinMode(SIT_DETECTOR_PIN, INPUT);
}

void determineTap() {
  int currentValue = analogRead(SIT_DETECTOR_PIN);

  if (tapState == 0 && currentValue < TAP_LOWER_LIMIT) {
    tapState = 1; 
  }
  
  if (tapState == 1 && currentValue > TAP_UPPER_LIMIT) {
    tapState = 0;
  }

  if (tapState != lastTapState && tapState == 1) {
    tapEventStart = now;
  }

  if (tapEventStart > 0 && now - tapEventStart < TAP_EVENT_DURATION) {
    int delta = now - tapEventStart;
    tapValue = map(delta, 0, TAP_EVENT_DURATION, 1024, 0);
  } else {
    tapValue = 0;
  }
  
  lastTapState = tapState;
}
