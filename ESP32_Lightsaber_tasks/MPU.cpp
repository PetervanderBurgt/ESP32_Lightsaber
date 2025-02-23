#include <Esp.h>
#include "MPU.h"
#include "pinConfig.h"
#include "globalVariables.h"

void MPUCode(void* pvParameters) {

  for (;;) {

    // Runs task every 25 MS
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}