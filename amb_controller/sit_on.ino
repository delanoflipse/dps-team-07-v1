
#define SIT_DETECTOR_PIN A0

void setupSitDetector() {
  // setup pins
  pinMode(SIT_DETECTOR_PIN, INPUT);
}

void determineSitOn() {
  int value = digitalRead(SIT_DETECTOR_PIN);

  sitOn = value == HIGH;
}
