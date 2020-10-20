#include "WT2003S_Player.h"

#define CALM_AUDIO "calm.mp3"
#define ACTIVE_AUDIO "active.mp3"
#define PICKUP_AUDIO "pickup.mp3"

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
int lastAudio = 0;

void setVolume(int vol) {
  if (lastVolume == vol) return;
  Mp3Player.volume(vol);
  lastVolume = vol;
}

void setAudio(int active, MachineState state) {
  int newAudio = state == 1
    ? 1
    : active ? 2 : 3;
  
  if (lastAudio == newAudio) return;
  switch (newAudio) {
    case 1:
    Mp3Player.playSDSong(PICKUP_AUDIO);
    Mp3Player.playMode(SINGLE_CYCLE);
    break;
    case 2:
    Mp3Player.playSDSong(ACTIVE_AUDIO);
    Mp3Player.playMode(SINGLE_CYCLE);
    break;
    case 3:
    Mp3Player.playSDSong(CALM_AUDIO);
    Mp3Player.playMode(SINGLE_CYCLE);
    break;
  }

//  Mp3Player.pause_or_play();
  lastAudio = newAudio;
}

void setupAudio() {
  COMSerial.begin(9600);
  Mp3Player.init(COMSerial);
  Mp3Player.playMode(SINGLE_CYCLE);
  
  setVolume(0);
  Mp3Player.pause_or_play();
}
