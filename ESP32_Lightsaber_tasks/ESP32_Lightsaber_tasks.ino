// this example will play a random track from all on the sd
//
// it expects the sd card to contain some mp3 files

#include <DFMiniMp3.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include "pinConfig.h"
#include "globalVariables.h"
#include "buttons.h"
#include "LEDs.h"
#include "MPU.h"
#include "DFPlayer.h"
#include "configMenu.h"

// FreeRTOS Configuration for Runtime Stats
#define configGENERATE_RUN_TIME_STATS 1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() (timer_start())  // Configure timer for runtime stats
#define portGET_RUN_TIME_COUNTER_VALUE() (millis())               // Use micros() for more precise time

DFPlayer audio(Serial1);
MovementDetection mpuClass;
Blade leds;
Buttons mainButton(button_double_main);
Buttons secondaryButton(button_double_secondary);
ConfigMenu menu = ConfigMenu();

bool print_stats = false;

global_states global_state = lightsaber_idle;
lightsaber_on_states lightsaber_on_state = lightsaber_on_boot;
config_states config_state = config_idle;

void printTask(void* pvParameters) {
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));  // Wait 1 second before printing stats
    printTaskStats();
  }
}

// Function to print FreeRTOS runtime stats
void printTaskStats() {
  char buffer[512];
  vTaskGetRunTimeStats(buffer);  // Get task runtime stats into the buffer
  DEBUG_PRINTLN("\nFreeRTOS Task Run Time Stats:");
  DEBUG_PRINTLN(buffer);  // Print the stats to the Serial Monitor
}

void setup() {
  Serial.begin(115200);
  menu.readConfig();

  mainButton.startTask();
  secondaryButton.startTask();
  vTaskDelay(100);

  mpuClass.startTask();
  vTaskDelay(100);

  leds.startTask();
  vTaskDelay(100);

  audio.startTask();
  vTaskDelay(100);

  if (print_stats) {
    // Create a task to print runtime stats every second
    xTaskCreatePinnedToCore(
      printTask,    /* Task function. */
      "PrintStats", /* name of task. */
      4096,         /* Stack size of task */
      NULL,         /* parameter of the task */
      1,            /* priority of the task */
      NULL,         /* Task handle to keep track of created task */
      1);           /* pin task to core 1 */
  }
}

void loop() {
  vTaskDelete(NULL);
}
