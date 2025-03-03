#include <Esp.h>
#include <DFMiniMp3.h>
#include "DFPlayer.h"
#include "pinConfig.h"
#include "globalVariables.h"

bool dfplayer_ready = false;
extern bool leds_ready;
extern bool mpu_ready;
extern bool buttons_ready;

extern global_states global_state;
extern lightsaber_on_states lightsaber_on_state;
extern uint8_t soundFont;

lightsaber_sounds current_sound = sound_unknown;

bool firstBoot = true;

DFPlayer::DFPlayer(HardwareSerial& serialPort)
  : dfmp3(serialPort) {
  // Any other initialization you need for MyPlayer
}

// Start the task by creating a FreeRTOS task
void DFPlayer::startTask() {
  // Create the task, passing `this` (the instance of the class) as the parameter
  xTaskCreatePinnedToCore(
    runTask,        /* Task function. */
    "DFPlayerTask", /* name of task. */
    2048,           /* Stack size of task */
    this,           /* parameter of the task */
    1,              /* priority of the task */
    NULL,           /* Task handle to keep track of created task */
    1);             /* pin task to core 1 */
}

// Static task function called by FreeRTOS
void DFPlayer::runTask(void* pvParameters) {
  // Cast the parameter to a pointer to the MyTaskClass instance
  DFPlayer* instance = static_cast<DFPlayer*>(pvParameters);

  // Call the instance's non-static method (the actual task)
  instance->DFPlayerCode();
}

void DFPlayer::DFPlayerCode() {
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
  dfplayer_ready = true;

  for (;;) {
    if (global_state == lightsaber_on) {
      switch (lightsaber_on_state) {
        case lightsaber_on_ignition:
          current_sound = getCurrentLightsaberTrack();
          if (current_sound != sound_poweron) {
            playLightsaberTrack(sound_poweron);
          }
          break;

        case lightsaber_on_retraction:
          current_sound = getCurrentLightsaberTrack();
          if (current_sound != sound_poweroff) {
            playLightsaberTrack(sound_poweroff);
          }
          break;
        case lightsaber_on_hum:
          current_sound = getCurrentLightsaberTrack();
          if (current_sound != sound_hum) {
            loopLightsaberTrack(sound_hum);
          }
          break;
        default:
          dfmp3.stop();
          current_sound = sound_unknown;
          break;
      }
    } else if (global_state == lightsaber_config) {
    } else {
      //global state is lightsaber_idle
      if (lightsaber_on_state == lightsaber_on_boot) {
        current_sound = getCurrentLightsaberTrack();
        if (current_sound != sound_boot) {
          playLightsaberTrack(sound_boot);
        }
        if (firstBoot) {
          Serial.println("Playing boot sound");
          firstBoot = false;
        } else {
          //Done playing boot sound
          if (dfmp3.getStatus().state == DfMp3_StatusState_Idle) {
            lightsaber_on_state = lightsaber_on_idle;
          }
        }
      }
    }
    dfmp3.loop();
    // Runs task every 20 MS
    vTaskDelay((1000 / FPS_DFPlayer) / portTICK_PERIOD_MS);
  }
}

void DFPlayer::playLightsaberTrack(lightsaber_sounds sound_to_play) {
  dfmp3.playGlobalTrack(fontAndEnumtoTrack(sound_to_play, soundFont));
}

void DFPlayer::loopLightsaberTrack(lightsaber_sounds sound_to_play) {
  dfmp3.loopGlobalTrack(fontAndEnumtoTrack(sound_to_play, soundFont));
}

lightsaber_sounds DFPlayer::getCurrentLightsaberTrack(){
  return getEnumFromGlobalTrack(dfmp3.getCurrentTrack());
}


uint16_t DFPlayer::fontAndEnumtoTrack(lightsaber_sounds sound, uint8_t soundFont) {
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

uint16_t DFPlayer::getGlobalTrackFromFolderandTrack(uint8_t folderInt, uint8_t trackInt) {
  if (folderInt == 1) {
    return trackInt;
  } else {
    return (folderInt - 2) * 30 + 29 + trackInt;
  }
  return 0;
}

lightsaber_sounds DFPlayer::getEnumFromGlobalTrack(uint16_t globalTrackInt) {
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

class DFPlayer::Mp3Notify {
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