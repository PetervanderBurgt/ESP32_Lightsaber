#include <Esp.h>
#include "MPU.h"
#include "pinConfig.h"
#include "globalVariables.h"

bool mpu_ready = false;

void MPUCode(void* pvParameters) {
  mpu_ready = true;
  for (;;) {

    // Runs task every 25 MS
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}