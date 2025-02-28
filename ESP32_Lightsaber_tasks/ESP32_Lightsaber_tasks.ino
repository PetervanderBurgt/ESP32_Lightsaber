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
DFPlayer audio(Serial1);

TaskHandle_t SerialTask;
TaskHandle_t ButtonsTask;
TaskHandle_t MPUTask;
TaskHandle_t LEDTask;

uint16_t globalTrackInt = 0;
uint8_t soundFont = 1;
global_states global_state = lightsaber_idle;
lightsaber_on_states lightsaber_on_state = lightsaber_on_boot;

void setup() {
  Serial.begin(115200);

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

  audio.startTask();
  vTaskDelay(100);
}

void loop() {
}
