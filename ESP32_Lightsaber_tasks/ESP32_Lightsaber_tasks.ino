// this example will play a random track from all on the sd
//
// it expects the sd card to contain some mp3 files

#include <DFMiniMp3.h>

#include "pinConfig.h"
#include "globalVariables.h"
#include "buttons.h"
#include "LEDs.h"
#include "MPU.h"
#include "DFPlayer.h"

// // instance a DfMp3 object,
// //
// DfMp3 dfmp3(Serial1);

TaskHandle_t DfPlayerTask;
TaskHandle_t SerialTask;
TaskHandle_t ButtonsTask;
TaskHandle_t MPUTask;
TaskHandle_t LEDTask;

uint16_t globalTrackInt = 0;
uint8_t soundFont = 1;
global_states global_state = lightsaber_idle;
lightsaber_on_states lightsaber_on_state = lightsaber_on_boot;

extern uint16_t fontAndEnumtoTrack(lightsaber_sounds sound, uint8_t soundFont);
extern uint16_t getGlobalTrackFromFolderandTrack(uint8_t folderInt, uint8_t trackInt);

void setup() {
  Serial.begin(115200);


  xTaskCreatePinnedToCore(
    SerialReadingCode, /* Task function. */
    "SerialReadTask",  /* name of task. */
    2048,              /* Stack size of task */
    NULL,              /* parameter of the task */
    1,                 /* priority of the task */
    &SerialTask,       /* Task handle to keep track of created task */
    1);                /* pin task to core 1 */
  vTaskDelay(100);

  xTaskCreatePinnedToCore(
    ButtonsCode,   /* Task function. */
    "ButtonsTask", /* name of task. */
    2048,          /* Stack size of task */
    NULL,          /* parameter of the task */
    1,             /* priority of the task */
    &ButtonsTask,  /* Task handle to keep track of created task */
    1);            /* pin task to core 1 */
  vTaskDelay(100);

  xTaskCreatePinnedToCore(
    MPUCode,   /* Task function. */
    "MPUTask", /* name of task. */
    2048,      /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &MPUTask,  /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */
  vTaskDelay(100);

  xTaskCreatePinnedToCore(
    LEDCode,   /* Task function. */
    "LEDTask", /* name of task. */
    2048,      /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &LEDTask,  /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */
  vTaskDelay(100);

  xTaskCreatePinnedToCore(
    DFPlayerCode,   /* Task function. */
    "DFPlayerTask", /* name of task. */
    2048,           /* Stack size of task */
    NULL,           /* parameter of the task */
    1,              /* priority of the task */
    &DfPlayerTask,  /* Task handle to keep track of created task */
    1);             /* pin task to core 1 */
  vTaskDelay(100);
}

void loop() {
}

void SerialReadingCode(void* pvParameters) {
  Serial.println("initializing...");
  for (;;) {
    if (Serial.available() > 0) {
      // Read the integer from the serial input
      uint8_t enum_number = Serial.readStringUntil('\n').toInt();
      lightsaber_sounds sound_to_play = static_cast<lightsaber_sounds>(enum_number);
      // Print the integer to the serial monitor
      Serial.print("You entered folder: ");
      Serial.println(enum_number);


      globalTrackInt = fontAndEnumtoTrack(sound_to_play, soundFont);
    }
    // Runs task every 25 MS
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}