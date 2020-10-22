#include "WT2003S_Player.h"

#define FOREST_AUDIO "b001.mp3"
#define ZEN_AUDIO "z001.mp3"
#define RAIN_AUDIO "r001.mp3"
#define FIRE_AUDIO "f001.mp3"

#define PICKUP_AUDIO "p001.mp3"
#define ALARM_AUDIO "a001.mp3"

#define TRANSITION_DURATION 1000

const int numberOfJazzSongs = 2;
String jazzSongs[numberOfJazzSongs] = { "j001.mp3", "j002.mp3" };
int jazzSongDurations[numberOfJazzSongs] = { 67813, 146917 };
int currentJazzSong = -1;
unsigned long lastSongStart;

#ifdef __AVR__
    #include <SoftwareSerial.h>
    SoftwareSerial SSerial(2, 3); // RX, TX
    #define COMSerial SSerial

    WT2003S<SoftwareSerial> Mp3Player;
#endif

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define COMSerial Serial1

    WT2003S<Uart> Mp3Player;
#endif

#ifdef ARDUINO_ARCH_STM32F4
    #define COMSerial Serial

    WT2003S<HardwareSerial> Mp3Player;
#endif

int lastVolume = 0;
String lastAudio = "none";
String newAudio = "none";

void setVolume(int vol) {
  // only update when value changes
  if (lastVolume == vol) {
    return;
  }
  
  Mp3Player.volume(vol);
  lastVolume = vol;
}

void setAudio(int active, Orientation orientation, MachineState state) {
  PLAY_MODE newPlayMode = SINGLE_CYCLE;
  // determine audio
  switch(orientation) {
    case up:
    // none
    newAudio = "none";
    break;
    
    case down:
    newAudio = RAIN_AUDIO;
    break;
    
    case right:
    newAudio = FOREST_AUDIO;
    break;
    
    case left:
    newAudio = FIRE_AUDIO;
    break;
    
    case front:
    newAudio = ZEN_AUDIO;
    break;
    
    case back:
    newPlayMode = SINGLE_SHOT;
    if (currentJazzSong > -1) {
      if (millis() > lastSongStart + jazzSongDurations[currentJazzSong] + TRANSITION_DURATION) {
         currentJazzSong = (currentJazzSong + 1) % numberOfJazzSongs;
      }
    } else {
      currentJazzSong = random(numberOfJazzSongs);
    }
    
    newAudio = jazzSongs[currentJazzSong];
    break;

    default:
    break;
  }
  
  switch(state) {
    case siton:
    case dorment:
    break;

    case error:
      newAudio = ALARM_AUDIO;
    break;
    
    case moving:
      newAudio = PICKUP_AUDIO;
    break;
    
    default:
    break;
  }

  // only update when value changes
  if (lastAudio == newAudio) {
    return;
  }

  if (!newAudio.startsWith("j")) {
    currentJazzSong = -1;
  }

  Serial.println(newAudio);
  
  if (newAudio != "none") {
    int error = Mp3Player.playSDSong(newAudio.c_str());
    Mp3Player.playMode(newPlayMode);
    lastSongStart = millis();

    if (error == -1) {
      Serial.println("Error playing audio!");
    }
  } else if (newAudio == "none") {
    int status = Mp3Player.getStatus();
    if (status == 1) {
      Mp3Player.pause_or_play();
    }
  }

  lastAudio = newAudio;
}

void setupAudio() {
  COMSerial.begin(9600);
  Mp3Player.init(COMSerial);
  setVolume(0);
  
  int status = Mp3Player.getDiskStatus();

  if (status == 0) {
    Serial.println("MP3 ERROR: no SD card!");
  } else if (status == 2) {
    Serial.println("MP3 ready!");
  } else {
    Serial.println("MP3 ERROR: unkown error");
  }

}
