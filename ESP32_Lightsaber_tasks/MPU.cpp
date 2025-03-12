#include <Esp.h>
#include "MPU.h"
#include "pinConfig.h"
#include "globalVariables.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

bool mpu_ready = false;

uint8_t swingSensitivity = 150;

MPU6050 mpu;

int const INTERRUPT_PIN = 4;  // Define the interruption #0 pin
bool blinkState;

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

/*------Interrupt detection routine------*/
volatile bool MPUInterrupt = false;  // Indicates whether MPU6050 interrupt pin has gone high
void DMPDataReady() {
  MPUInterrupt = true;
}

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
  DEBUG_PRINT("MPUTask running on core ");
  DEBUG_PRINTLN(xPortGetCoreID());
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS((1000 / MPU_HZ));

  //Connect and start i2c
  Wire.begin();
  Wire.setClock(400000);  // 400kHz I2C clock. Comment on this line if having compilation difficulties
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  /*Verify connection*/
  DEBUG_PRINTLN(F("Testing MPU6050 connection..."));
  if (mpu.testConnection() == false) {
    DEBUG_PRINTLN("MPU6050 connection failed");
    while (true)
      ;
  } else {
    DEBUG_PRINTLN("MPU6050 connection successful");
  }

  /* Initializate and configure the DMP*/
  DEBUG_PRINTLN(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // Set Digital Low and High pass filter
  mpu.setDLPFMode(3);
  mpu.setDHPFMode(0);

  // set interrupt when motion is detected
  mpu.setIntMotionEnabled(true);
  // set inteerupt off for when no movement detected
  mpu.setIntZeroMotionEnabled(false);

  mpu.setMotionDetectionThreshold(10);
  mpu.setMotionDetectionDuration(2);

  /* Supply your gyro offsets here, read with MPU zero example */
  mpu.setXAccelOffset(-1226);
  mpu.setYAccelOffset(-2412);
  mpu.setZAccelOffset(656);
  mpu.setXGyroOffset(132);
  mpu.setYGyroOffset(-15);
  mpu.setZGyroOffset(11);


  /* Making sure it worked (returns 0 if so) */
  if (devStatus == 0) {
    DEBUG_PRINTLN("These are the Active offsets: ");
    mpu.PrintActiveOffsets();
    DEBUG_PRINTLN(F("Enabling DMP..."));  //Turning ON DMP
    mpu.setDMPEnabled(true);

    /*Enable Arduino interrupt detection*/
    DEBUG_PRINT(F("Enabling interrupt detection (Arduino external interrupt "));
    DEBUG_PRINT(digitalPinToInterrupt(INTERRUPT_PIN));
    DEBUG_PRINTLN(F(")..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), DMPDataReady, RISING);
    MPUIntStatus = mpu.getIntStatus();

    /* Set the DMP Ready flag so the main loop() function knows it is okay to use it */
    DEBUG_PRINTLN(F("DMP ready! Waiting for first interrupt..."));
    DMPReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();  //Get expected DMP packet size for later comparison
  } else {
    DEBUG_PRINT(F("DMP Initialization failed (code "));  //Print the error code
    DEBUG_PRINT(devStatus);
    DEBUG_PRINTLN(F(")"));
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
  }

  mpu_ready = true;

  xLastWakeTime = xTaskGetTickCount();
  for (;;) {

    if (!DMPReady) return;  // Stop the program if DMP programming fails.

    if (MPUInterrupt) {
      MPUInterrupt = false;  //reset interrupt flag

      // An interrupt has occurred.  get INT_STATUS byte
      //pulse an output pin to measure getIntStatus() duration
      MPUIntStatus = mpu.getIntStatus();

      // get current FIFO count
      fifoCount = mpu.getFIFOCount();

      // check for overflow (this should never happen unless our code is too inefficient)
      if ((MPUIntStatus & bit(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        //  fifoCount = mpu.getFIFOCount();  // will be zero after reset no need to ask
        Serial.println(F("FIFO overflow!"));

        // otherwise, check for DMP data ready interrupt (this should happen frequently)
      } else if (MPUIntStatus & bit(MPU6050_INTERRUPT_DMP_INT_BIT)) {
        // read all available packets from FIFO
        while (fifoCount >= packetSize)  // Lets catch up to NOW, in case someone is using the dreaded delay()!
        {
          mpu.getFIFOBytes(FIFOBuffer, packetSize);
          // track FIFO count here in case there is > 1 packet available
          // (this lets us immediately read more without waiting for an interrupt)
          fifoCount -= packetSize;
        }
      }
      /* Display real acceleration, adjusted to remove gravity */
      mpu.dmpGetQuaternion(&q, FIFOBuffer);
      mpu.dmpGetAccel(&aa, FIFOBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
      // DEBUG_PRINT("areal\t");
      // DEBUG_PRINT(aaReal.x);
      // DEBUG_PRINT("\t");
      // DEBUG_PRINT(aaReal.y);
      // DEBUG_PRINT("\t");
      // DEBUG_PRINTLN(aaReal.z);
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}