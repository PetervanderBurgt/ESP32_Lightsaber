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

DFPlayer audio(Serial1);
MovementDetection mpu;
Blade leds;
Buttons mainButton(button_double_main);
Buttons secondaryButton(button_double_secondary);

uint8_t soundFont = 1;
global_states global_state = lightsaber_idle;
lightsaber_on_states lightsaber_on_state = lightsaber_on_boot;
config_states config_state = config_idle;

void setup() {
  Serial.begin(115200);

  mainButton.startTask();
  secondaryButton.startTask();
  vTaskDelay(100);

  mpu.startTask();
  vTaskDelay(100);

  leds.startTask();
  vTaskDelay(100);

  audio.startTask();
  vTaskDelay(100);
}

void loop() {
}
