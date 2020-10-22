#include <SPI.h>
#include <WiFiNINA.h>
#include <MQTT.h>

WiFiClient net;
MQTTClient client;

const char ssid[] = WIFI_SSID;
const char pass[] = WIFI_PASS;

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if (topic == "/log") {
    Serial.println("Log: " + payload);
  } else if (topic.startsWith("/amb-action")) {
    String action = payload.substring(0, 1);
    int actionId = action.toInt();
    String arg = payload.substring(2, payload.length());
    Serial.println("Log: action " + action + " arg: " + arg);

    switch(actionId) {
      // setState
      case 0:
      if (arg == "reset") {
        forceState = false;
      } else {
        forceState = true;
      }
 
      if (arg == "siton") { currentState = siton; }
      else if (arg == "moving") { currentState = moving; }
      else if (arg == "error") { currentState = error; }
      else if (arg == "dorment") { currentState = dorment; }
      
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
}

void loopWiFi() {
  client.loop();

  if (!client.connected()) {
    connectWiFi();
  }
}
