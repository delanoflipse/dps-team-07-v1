
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
    return "dormet";
    case moving:
    return "moving";
    case error:
    return "error";
    case siton:
    return "sit_on";
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
