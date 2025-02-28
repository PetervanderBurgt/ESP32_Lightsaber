#include <Esp.h>
#include "MPU.h"
#include "pinConfig.h"
#include "globalVariables.h"

bool mpu_ready = false;


MovementDetection::MovementDetection() {
  // Any other initialization you need for this class
}

// Start the task by creating a FreeRTOS task
void MovementDetection::startTask() {
  // Create the task, passing `this` (the instance of the class) as the parameter

  xTaskCreatePinnedToCore(
    runTask,   /* Task function. */
    "MPUTask", /* name of task. */
    2048,      /* Stack size of task */
    this,      /* parameter of the task */
    1,         /* priority of the task */
    NULL,      /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */
}

// Static task function called by FreeRTOS
void MovementDetection::runTask(void* pvParameters) {
  // Cast the parameter to a pointer to the MyTaskClass instance
  MovementDetection* instance = static_cast<MovementDetection*>(pvParameters);

  // Call the instance's non-static method (the actual task)
  instance->MPUCode();
}

void MovementDetection::MPUCode() {
  Serial.print("MPUTask running on core ");
  Serial.println(xPortGetCoreID());
  mpu_ready = true;
  for (;;) {

    vTaskDelay((1000 / FPS_MPU) / portTICK_PERIOD_MS);
  }
}