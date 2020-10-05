#include "WT2003S_Player.h"

#define CALM_AUDIO "calm.mp3"
#define ACTIVE_AUDIO "active.mp3"

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

void setVolume(int vol) {
  Mp3Player.volume(vol);
}

void playSong(int active) {
  Mp3Player.playSDSong(active ? ACTIVE_AUDIO : CALM_AUDIO);
//  Mp3Player.pause_or_play();
}

void setupAudio() {
  COMSerial.begin(9600);
  Mp3Player.init(COMSerial);
  Mp3Player.playMode(SINGLE_CYCLE);
  
  setVolume(40);
  Mp3Player.pause_or_play();
}
