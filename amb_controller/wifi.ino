/**
 * @file wifi.ino
 * @copyright Delano Flipse 2020
 * WiFi communication using Shiftr.io pub/sub.
 */
 
#include <SPI.h>
#include <WiFiNINA.h>
#include <MQTT.h>

WiFiClient net;
MQTTClient client;

const char ssid[] = WIFI_SSID;
const char pass[] = WIFI_PASS;

void messageReceived(String &topic, String &payload) {
  if (topic == "/log") {
    Serial.println("Log: " + payload);
  } else if (topic == "/amb-global-distance") {
    int dist = payload.toInt();
    closestDevice = dist;
  } else if (topic == "/amb-max-volume") {
    int vol = payload.toInt();
    maxVolume = vol;
  } else if (topic == "/amb-min-volume") {
    int vol = payload.toInt();
    minVolume = vol;
  } else if (topic.startsWith("/amb-action")) {
    String action = payload.substring(0, 1);
    int actionId = action.toInt();
    String arg = payload.substring(2, payload.length());
    Serial.println("Log: action " + action + " arg: " + arg);

    switch(actionId) {
      // setState
      case 0:
 
      if (arg == "active") { currentState = active; }
      else if (arg == "dorment") { currentState = dorment; }
      else if (arg == "quiet") { currentState = quiet; }
      
      break;
      // setClosestOrientation
      case 1:
 
      if (arg == "up") { closestOrientation = up; }
      else if (arg == "down") { closestOrientation = down; }
      else if (arg == "right") { closestOrientation = right; }
      else if (arg == "left") { closestOrientation = left; }
      else if (arg == "front") { closestOrientation = front; }
      else if (arg == "back") { closestOrientation = back; }
      
      break;
    }
  }
}

void connectWiFi() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("amb-arduino", SHIFTR_KEY, SHIFTR_SECRET)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
}

void logWiFi(String str) {
  client.publish("/log-device", str);
}

void setupWiFi() {
  // CONNECT
  
  WiFi.begin(ssid, pass);

  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);

  connectWiFi();

  // SUBSCRIBE
  
  client.subscribe("/log");
  client.subscribe("/amb-action-" + deviceName);
  client.subscribe("/amb-global-distance");
  client.subscribe("/amb-max-volume");
  client.subscribe("/amb-min-volume");
}

void loopWiFi() {
  client.loop();

  if (!client.connected()) {
    connectWiFi();
  }
}
