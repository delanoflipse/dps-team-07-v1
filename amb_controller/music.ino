/**
 * @file music.ino
 * @copyright Delano Flipse 2020
 * Randomised continues audio player based on state.
 */
 
#include "WT2003S_Player.h"

struct AudioCollection {
  int size;
  int index;
  char prefix[2];
};

#define ALARM_AUDIO "AL01.mp3"

struct AudioCollection forestAudios = { 1, -1, { 'F', 'S' } };
struct AudioCollection zenAudios = { 2, -1, { 'Z', 'N' } };
struct AudioCollection fireAudios = { 2, -1, { 'F', 'R' } };
struct AudioCollection jazzAudios = { 5, -1, { 'J', 'Z' } };
struct AudioCollection rainAudios = { 2, -1, { 'R', 'N' } };

struct AudioCollection * lastCollection = NULL;

// #define TRANSITION_DURATION 1000

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

int lastVolume = -1;
String lastAudio = "none";
String newAudio = "none";
PLAY_MODE playMode = SINGLE_CYCLE;
int playStatus = 0;

unsigned long lastCheck;

void setVolume(int vol) {
  // only update when value changes
  if (lastVolume == vol) {
    return;
  }
  
  Mp3Player.volume(vol);
  lastVolume = vol;
}

String setNewAudio(struct AudioCollection * collection) {
  bool playing = playStatus == 1;
  bool newCollection = lastCollection == NULL
    || collection->index == -1
    || lastCollection->prefix[0] != collection->prefix[0]
    || lastCollection->prefix[1] != collection->prefix[1];

  if (collection->size > 1) {
    playMode = SINGLE_SHOT;
    if (newCollection) {
      collection->index = random(collection->size) + 1;
    } else if (!playing) {
      collection->index = collection->index == collection->size ? 1 : collection->index + 1;
    }
  } else {
    playMode = SINGLE_CYCLE;
    collection->index = 1;
  }

  String songName = String(collection->prefix[0]) + String(collection->prefix[1]);
  songName += collection->index < 10 ? "0" + String(collection->index) : String(collection->index);
  songName += ".mp3";
  return songName;
}

void setAudio(Orientation orientation, Proximity proximity, MachineState state, MachineState lastState) {
  // move volume to the target volume
//  if (targetVolume != currentVolume) {
//    currentVolume += targetVolume > currentVolume ? 1 : -1;
//  }
  setVolume(targetVolume);

  // warn: does this influence performance?
  if (lastCheck + 1000 > now) {
    playStatus = Mp3Player.getStatus();
    lastCheck = now;
  }

  // determine audio
  switch(orientation) {
    case up:
    newAudio = setNewAudio(&forestAudios);
    lastCollection = &forestAudios;
    break;
    
    case down:
    // none
    newAudio = "none";
    lastCollection = NULL;
    break;
    
    case right:
    newAudio = setNewAudio(&rainAudios);
    lastCollection = &rainAudios;
    break;
    
    case left:
    newAudio = setNewAudio(&fireAudios);
    lastCollection = &fireAudios;
    break;
    
    case front:
    newAudio = setNewAudio(&jazzAudios);
    lastCollection = &jazzAudios;
    break;
    
    case back:
    newAudio = setNewAudio(&zenAudios);
    lastCollection = &zenAudios;
    break;

    default:
    break;
  }
  
  int scaledValue = map(closestDevice, CLOSEST_DEVICE_ERROR_MIN_DISTANCE, CLOSEST_DEVICE_GROUP_MIN_DISTANCE, minVolume, maxVolume);
  switch(state) {
    case active:
      targetVolume = constrain(scaledValue, 0, 30);
    break;

    case dorment:
    case quiet:
      targetVolume = 0;
    break;

    case error:
      if (lastStateChange + ALARM_REPLAYS * 2400 > now) {
        playMode = SINGLE_CYCLE;
        newAudio = ALARM_AUDIO;
        targetVolume = maxVolume;
        currentVolume = maxVolume;
      } else {
        targetVolume = minVolume;
        currentVolume = minVolume;
      }
    break;
    
    default:
    break;
  }

  // only update when value changes
  if (lastAudio == newAudio) {
    return;
  }

  // todo: reset other audios
  Serial.println(newAudio);
  
  if (newAudio == "none") {
    if (playStatus == 1) {
      Mp3Player.pause_or_play();
    }
    playStatus = 0;
  } else {
    
    int error = Mp3Player.playSDSong(newAudio.c_str());
    Mp3Player.playMode(playMode);

    if (error == -1) {
      Serial.println("Error playing audio!");
    } else {
      playStatus = 1;
    }
  }

  lastAudio = newAudio;
}

void setupAudio() {
  COMSerial.begin(9600);
  Mp3Player.init(COMSerial);
  setVolume(0);
  lastCheck = millis();
  
  int status = Mp3Player.getDiskStatus();

  if (status == 0) {
    Serial.println("MP3 ERROR: no SD card!");
  } else if (status == 2) {
    Serial.println("MP3 ready!");
  } else {
    Serial.println("MP3 ERROR: unkown error");
  }

}
