#include <Esp.h>
#include <DFMiniMp3.h>
#include "DFPlayer.h"
#include "pinConfig.h"
#include "globalVariables.h"

extern lightsaber_on_states lightsaber_on_state;
extern uint16_t globalTrackInt;
extern uint8_t soundFont;
lightsaber_sounds current_sound = sound_unknown;

// instance a DfMp3 object,
DfMp3 dfmp3(Serial1);

void DFPlayerCode(void* pvParameters) {
  Serial.print("DFPlayerTask running on core ");
  Serial.println(xPortGetCoreID());
  dfmp3.begin(/*rx =*/RX_DFPLAYER, /*tx =*/TX_DFPLAYER);
  // for boards that support hardware arbitrary pins
  // dfmp3.begin(10, 11); // RX, TX

  // during development, it's a good practice to put the module
  // into a known state by calling reset().
  // You may hear popping when starting and you can remove this
  // call to reset() once your project is finalized
  dfmp3.reset();

  uint16_t version = dfmp3.getSoftwareVersion();
  Serial.print("version ");
  Serial.println(version);

  // show some properties and set the volume
  uint16_t volume = dfmp3.getVolume();
  Serial.print("volume ");
  Serial.println(volume);
  dfmp3.setVolume(30);

  uint16_t count = dfmp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
  Serial.print("files ");
  Serial.println(count);

  uint16_t mode = dfmp3.getPlaybackMode();
  Serial.print("playback mode ");
  Serial.println(mode);

  Serial.println("starting...");
  for (;;) {
    if (globalTrackInt != 0) {
      Serial.print("globalTrackInt DF: ");
      Serial.println(globalTrackInt);

      // dfmp3.playGlobalTrack(globalTrackInt);
      dfmp3.loopGlobalTrack(globalTrackInt);
      globalTrackInt = 0;
    }

    Serial.print("Current playing track= ");
    Serial.println(current_sound);

    switch (lightsaber_on_state) {
      case lightsaber_on_ignition:
        current_sound = getEnumFromGlobalTrack(dfmp3.getCurrentTrack());
        if (current_sound != sound_poweron) {
          dfmp3.playGlobalTrack(fontAndEnumtoTrack(sound_poweron, soundFont));
        }
        break;

      case lightsaber_on_retraction:
          current_sound = getEnumFromGlobalTrack(dfmp3.getCurrentTrack());
        if (current_sound != sound_poweroff) {
          dfmp3.playGlobalTrack(fontAndEnumtoTrack(sound_poweroff, soundFont));
        }
        break;

      case lightsaber_on_hum:
          current_sound = getEnumFromGlobalTrack(dfmp3.getCurrentTrack());
        if (current_sound != sound_hum) {
          dfmp3.loopGlobalTrack(fontAndEnumtoTrack(sound_hum, soundFont));
        }
        break;

      default:
        dfmp3.stop();
        current_sound = sound_unknown;
        break;
    }

    dfmp3.loop();
    // Runs task every 25 MS
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

uint16_t fontAndEnumtoTrack(lightsaber_sounds sound, uint8_t soundFont) {
  uint8_t track = 0;
  switch (sound) {
    case sound_boot:
      track = 1;
      break;
    case sound_poweron:
      track = random(2, 5 + 1);
      break;
    case sound_poweroff:
      track = random(6, 7 + 1);
      break;
    case sound_swing:
      track = random(8, 15 + 1);
      break;
    case sound_clash:
      track = random(16, 23 + 1);
      break;
    case sound_lockup:
      track = 24;
      break;
    case sound_blaster:
      track = random(25, 28 + 1);
      break;
    case sound_font:
      track = 29;
      break;
    default:
    case sound_hum:
      track = 30;
      break;
  }
  return getGlobalTrackFromFolderandTrack(soundFont + 1, track);
}

uint16_t getGlobalTrackFromFolderandTrack(uint8_t folderInt, uint8_t trackInt) {
  if (folderInt == 1) {
    return trackInt;
  } else {
    return (folderInt - 2) * 30 + 29 + trackInt;
  }
  return 0;
}

lightsaber_sounds getEnumFromGlobalTrack(uint16_t globalTrackInt) {
  if (globalTrackInt < 29) {
    return sound_config;
  } else {
    uint8_t fontTrack = ((globalTrackInt - 29) - 1) % 30;
    fontTrack = fontTrack + 1;  // correct for annoying 1 to 30 counting
    if (fontTrack == 1) return sound_boot;
    if (2 <= fontTrack && fontTrack <= 5) return sound_poweron;
    if (6 <= fontTrack && fontTrack <= 7) return sound_poweroff;
    if (8 <= fontTrack && fontTrack <= 15) return sound_swing;
    if (16 <= fontTrack && fontTrack <= 23) return sound_clash;
    if (fontTrack == 24) return sound_lockup;
    if (25 <= fontTrack && fontTrack <= 28) return sound_blaster;
    if (fontTrack == 29) return sound_font;
    if (fontTrack == 30) return sound_hum;
  }
  return sound_unknown;
}

class Mp3Notify {
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action) {
    if (source & DfMp3_PlaySources_Sd) {
      Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) {
      Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) {
      Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode) {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Communication Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track) {
    Serial.print("Play finished for #");
    Serial.println(track);
  }
  static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "removed");
  }
};