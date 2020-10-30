/**
 * @file util.ino
 * @copyright Delano Flipse 2020
 * Reusable utilities
 */

 
void fillArray(float arr[], const int len, float defaultValue) {
  for (int i = 0; i < len; i++) {
    arr[i] = defaultValue;
  }
}

void fillArray(int arr[], const int len, int defaultValue) {
  for (int i = 0; i < len; i++) {
    arr[i] = defaultValue;
  }
}

float runningAverage(float arr[], const int len, float newValue) {
  float total = newValue;
  for (int i = 1; i < len; i++) {
    total += arr[i];
    arr[i-1] = arr[i];
  }

  arr[len - 1] = newValue;
  return total / len;
}

float runningAverage(int arr[], const int len, int newValue) {
  float total = (float)newValue;
  for (int i = 1; i < len; i++) {
    total += (float)arr[i];
    arr[i-1] = arr[i];
  }

  arr[len - 1] = newValue;
  return (float)total / len;
}


float runningMaximum(int arr[], const int len, int newValue) {
  float ret = (float)newValue;
  
  for (int i = 1; i < len; i++) {
    ret = max(ret, (float)arr[i]);
    arr[i-1] = arr[i];
  }

  arr[len - 1] = newValue;
  
  return ret;
}

String stateToString(MachineState state) {
  switch (state) {
    case dorment:
    return "dorment";
    case error:
    return "error";
    case active:
    return "active";
    case quiet:
    return "quiet";
    default:
    return "unkown";
  } 
}

String HSLtoString(struct HSL hsl) {
//  int h = (int) hsl.hue * 360;
//  int s = (int) hsl.saturation * 100;
//  int l = (int) hsl.lightness * 100;
  float h = hsl.hue;
  float s = hsl.saturation;
  float l = hsl.lightness;
  return String(h) + "h " + String(s) + "s " + String(l);
}

String proximityToString(Proximity proximity) {
  switch (proximity) {
    case close:
    return "too_close";
    case group:
    return "in_group";
    case alone:
    return "alone";
    default:
    return "unkown";
  } 
}

String orientationToString(Orientation orient) {
  switch (orient) {
    case left:
    return "left";
    case right:
    return "right";
    case front:
    return "front";
    case back:
    return "back";
    case up:
    return "up";
    case down:
    return "down";
    case none:
    return "none";
    default:
    return "unkown";
  } 
}
