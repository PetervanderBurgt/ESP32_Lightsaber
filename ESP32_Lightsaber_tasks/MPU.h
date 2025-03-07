#ifndef MPU_H
#define MPU_H

class MovementDetection {
private:
  // Code for movement objects

  // Code for task creation and running
  static void runTask(void* pvParameters);
  void MPUCode();
  TimerHandle_t MPUtimer;
  static void IRAM_ATTR mpuTimerCallback(TimerHandle_t xTimer);

  // Code for helper functions in tasks

public:
  // Code for movement objects
  MovementDetection();
  // Code for task creation and running
  void startTask();

  // Code for helper functions in tasks
};

extern bool mpu_ready;

#endif