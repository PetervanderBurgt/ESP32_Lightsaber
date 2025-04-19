#include <Esp.h>
#include "MPU.h"
#include "pinConfig.h"
#include "globalVariables.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

extern global_states global_state;
extern lightsaber_on_states lightsaber_on_state;

extern bool blaster_enabled;
extern bool lockup_enabled;
extern uint8_t effectLeds;
extern uint8_t effectLedsLength;

bool mpu_ready = false;

uint16_t swingSensitivity = 960;  // range should be between 0 and 16000 with increments of 160

MPU6050 mpu;

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

/*------Interrupt detection routine------*/
volatile bool MPUDataReady = false;  // Indicates whether MPU6050 interrupt pin has gone high
void DMPDataReady() {
  //Data ready, make sure to read
  MPUDataReady = true;
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

void MovementDetection::initMPU() {
  //Connect and start i2c
  Wire.begin();
  Wire.setClock(400000);  // 400kHz I2C clock. Comment on this line if having compilation difficulties
  mpu.initialize();
  pinMode(MPU_INTERRUPT, INPUT);

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
  mpu.setDHPFMode(4);

  mpu.setMotionDetectionThreshold(CLASH_THRESHOLD);
  mpu.setMotionDetectionDuration(CLASH_DURATION);
  mpu.setInterruptMode(false);
  mpu.setInterruptLatch(true);
  // set interrupt when motion is detected
  mpu.setIntMotionEnabled(true);

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
    DEBUG_PRINT(digitalPinToInterrupt(MPU_INTERRUPT));
    DEBUG_PRINTLN(F(")..."));
    attachInterrupt(digitalPinToInterrupt(MPU_INTERRUPT), DMPDataReady, RISING);
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
}

void MovementDetection::MPUCode() {
  DEBUG_PRINT("MPUTask running on core ");
  DEBUG_PRINTLN(xPortGetCoreID());
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS((1000 / MPU_HZ));
  initMPU();

  mpu_ready = true;

  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    // This code makes sure to read the fifo buffer of the MPU to always have the latest data
    if (MPUDataReady) {
      MPUDataReady = false;  //reset interrupt flag
      MPUIntStatus = mpu.getIntStatus();

      // Only execute application code if lightsaber is on
      if (global_state == lightsaber_on) {
        handleClash();

        handleSwing();
      }
      // This block makes sure that the fifo is up to date and read correctly
    }

    readMPUData();

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void MovementDetection::handleClash() {
  bool clashInt = (MPUIntStatus >> 6) && 0x1;  // Only check the motion bit

  // This is only done when the motion interrupt pin of the interrupt status is set, which can be configured by
  // setMotionDetectionThreshold and setMotionDetectionDuration
  if (clashInt && !clashTriggered && !blastTriggered && !swingTriggered && !lockupTriggered) {

    if (lockup_enabled) {
      lockup_enabled = false;

      lockupTriggered = true;
      startLockupMillis = millis();
      lightsaber_on_state = lightsaber_on_bladelockup;
    } else {
      /* Display real acceleration, adjusted to remove gravity */
      DEBUG_PRINT("areal\t");
      DEBUG_PRINT(aaReal.x);
      DEBUG_PRINT("\t");
      DEBUG_PRINT(aaReal.y);
      DEBUG_PRINT("\t");
      DEBUG_PRINTLN(aaReal.z);
      DEBUG_PRINTLN("CLASH DETECTED");

      clashTriggered = true;
      startClashMillis = millis();
      lightsaber_on_state = lightsaber_on_clash;
    }
  }

  if (lockupTriggered && millis() - startLockupMillis > LOCKUP_FX_DURATION) {
    lockupTriggered = false;
    lightsaber_on_state = lightsaber_on_hum;
  }

  if (clashTriggered && millis() - startClashMillis > CLASH_FX_DURATION) {
    clashTriggered = false;
    lightsaber_on_state = lightsaber_on_hum;
  }
}

void MovementDetection::handleSwing() {
  // This block should house something to detect motion and swings, not clashes
  bool motionInt = abs(aaReal.x) > swingSensitivity || abs(aaReal.y) > swingSensitivity || abs(aaReal.z) > swingSensitivity;
  if (motionInt && !clashTriggered && !blastTriggered && !swingTriggered && !lockupTriggered) {

    if (blaster_enabled) {
      blaster_enabled = false;

      blastTriggered = true;
      startBlastMillis = millis();
      effectLeds = random(effectLedsLength, NUM_LEDS);

      lightsaber_on_state = lightsaber_on_blasterdeflect;
    } else {
      /* Display real acceleration, adjusted to remove gravity */
      DEBUG_PRINT("swingSensitivity\t");
      DEBUG_PRINT(swingSensitivity);
      DEBUG_PRINT("areal\t");
      DEBUG_PRINT(abs(aaReal.x) > swingSensitivity);
      DEBUG_PRINT("\t");
      DEBUG_PRINT(abs(aaReal.y) > swingSensitivity);
      DEBUG_PRINT("\t");
      DEBUG_PRINTLN(abs(aaReal.z) > swingSensitivity);
      DEBUG_PRINTLN("MOTION DETECTED");

      swingTriggered = true;
      startSwingMillis = millis();
      lightsaber_on_state = lightsaber_on_swing;
    }
  }

  if (blastTriggered && millis() - startBlastMillis > BLASTER_FX_DURATION) {
    blastTriggered = false;
    lightsaber_on_state = lightsaber_on_hum;
  }

  if (swingTriggered && millis() - startSwingMillis > SWING_FX_DURATION) {
    swingTriggered = false;
    lightsaber_on_state = lightsaber_on_hum;
  }
}

void MovementDetection::readMPUData() {
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
  mpu.dmpGetQuaternion(&q, FIFOBuffer);
  mpu.dmpGetAccel(&aa, FIFOBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
}
