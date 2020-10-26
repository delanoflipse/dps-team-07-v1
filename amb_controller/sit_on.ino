/**
 * @file sit_on.ino
 * @copyright Delano Flipse 2020
 * ! Work in progress
 * A toggle for user presence.
 */
 
#define SIT_DETECTOR_PIN A0

int lastValue = 0;
void setupSitDetector() {
  // setup pins
  pinMode(SIT_DETECTOR_PIN, INPUT);
}

void determineSitOn() {
  int currentValue = digitalRead(SIT_DETECTOR_PIN);

  if (currentValue != lastValue && currentValue == HIGH) {
    sitOn = !sitOn;
  }
 
  lastValue = currentValue;
}
