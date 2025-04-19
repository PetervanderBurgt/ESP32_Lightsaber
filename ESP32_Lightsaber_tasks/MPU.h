#ifndef MPU_H
#define MPU_H

class MovementDetection {
private:
  // Code for movement objects

  // Code for task creation and running
  static void runTask(void* pvParameters);
  void MPUCode();

  // Code for helper functions in tasks
  void initMPU();
  void handleClash();
  void handleSwing();
  void readMPUData();

public:
  // Code for movement objects
  MovementDetection();
  // Code for task creation and running
  void startTask();

  // Code for helper functions in tasks
};

extern bool mpu_ready;

#endif