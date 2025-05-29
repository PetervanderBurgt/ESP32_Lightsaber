#ifndef MPU_H
#define MPU_H
#include "MPU6050_6Axis_MotionApps20.h"

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

  bool clashTriggered = false;
  bool swingTriggered = false;
  bool blastTriggered = false;
  bool lockupTriggered = false;
  uint32_t startClashMillis = 0;
  uint32_t startBlastMillis = 0;
  uint32_t startLockupMillis = 0;
  uint32_t startSwingMillis = 0;

  /*---MPU6050 Control/Status Variables---*/
  bool DMPReady = false;   // Set true if DMP init was successful
  uint8_t MPUIntStatus;    // Holds actual interrupt status byte from MPU
  uint8_t devStatus;       // Return status after each device operation (0 = success, !0 = error)
  uint16_t packetSize;     // Expected DMP packet size (default is 42 bytes)
  uint16_t fifoCount;      // count of all bytes currently in FIFO
  uint8_t FIFOBuffer[64];  // FIFO storage buffer

  /*---Orientation/Motion Variables---*/
  Quaternion q;         // [w, x, y, z]         Quaternion container
  VectorInt16 aa;       // [x, y, z]            Accel sensor measurements
  VectorInt16 aaReal;   // [x, y, z]            Gravity-free accel sensor measurements
  VectorFloat gravity;  // [x, y, z]            Gravity vector

public:
  // Code for movement objects
  MovementDetection();
  // Code for task creation and running
  void startTask();

  // Code for helper functions in tasks
};

extern bool mpu_ready;

#endif