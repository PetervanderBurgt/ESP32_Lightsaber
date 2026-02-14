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
    runTask,             /* Task function. */
    "MPUTask",           /* name of task. */
    MPU_TASK_STACK_SIZE, /* Stack size of task */
    this,                /* parameter of the task */
    MPU_TASK_PRIORITY,   /* priority of the task */
    NULL,                /* Task handle to keep track of created task */
    1);                  /* pin task to core 1 */
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
  // TODO Check if this clock speed change helped
  Wire.setClock(100000);  // 400kHz I2C clock. Comment on this line if having compilation difficulties
  Wire.setTimeout(3);     //timeout value in mSec, retrieved from https://www.tweaking4all.com/forum/arduino/problem-using-mpu-6050-accel-gyrp-with-esp32/paged/5/#post-2666
  mpu.initialize();
  pinMode(MPU_INTERRUPT, INPUT_PULLUP);

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

  // Set output rate safely
  const uint8_t desiredRate = 20;                   // Hz
  const uint8_t divider = (200 / desiredRate) - 1;  // 200Hz base
  mpu.setRate(divider);

  /* Supply your gyro offsets here, read with MPU zero example */
  mpu.setXAccelOffset(X_ACCEL_OFFSET);
  mpu.setYAccelOffset(Y_ACCEL_OFFSET);
  mpu.setZAccelOffset(Z_ACCEL_OFFSET);
  mpu.setXGyroOffset(X_GYRO_OFFSET);
  mpu.setYGyroOffset(Y_GYRO_OFFSET);
  mpu.setZGyroOffset(Z_GYRO_OFFSET);


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


      readMPUData();

      // Only execute application code if lightsaber is on
      if (global_state == lightsaber_on) {
        handleClash();

        handleSwing();
      }
      // DEBUG_PRINTLN("MPU RUNNING");
      // This block makes sure that the fifo is up to date and read correctly
    }
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void MovementDetection::handleClash() {
  bool clashInt = ((MPUIntStatus >> 6) & 0x1) != 0;  // Only check the motion bit

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
      // DEBUG_PRINT("areal\t");
      // DEBUG_PRINT(aaReal.x);
      // DEBUG_PRINT("\t");
      // DEBUG_PRINT(aaReal.y);
      // DEBUG_PRINT("\t");
      // DEBUG_PRINTLN(aaReal.z);
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
  const int16_t upperThreshold = 20000;            // new upper limit
  bool motionInt =
    ((abs(aaReal.x) > swingSensitivity) && (abs(aaReal.x) < upperThreshold)) || 
    ((abs(aaReal.y) > swingSensitivity) && (abs(aaReal.y) < upperThreshold)) || 
    ((abs(aaReal.z) > swingSensitivity) && (abs(aaReal.z) < upperThreshold));
  if (motionInt && !clashTriggered && !blastTriggered && !swingTriggered && !lockupTriggered) {

    if (blaster_enabled) {
      blaster_enabled = false;

      blastTriggered = true;
      startBlastMillis = millis();
      effectLeds = random(effectLedsLength, NUM_LEDS);

      lightsaber_on_state = lightsaber_on_blasterdeflect;
    } else {
      /* Display real acceleration, adjusted to remove gravity */
      // DEBUG_PRINT("swingSensitivity\t");
      // DEBUG_PRINTLN(swingSensitivity);
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
  uint8_t intStatus = mpu.getIntStatus();
  fifoCount = mpu.getFIFOCount();
  // check for overflow (this should never happen unless our code is too inefficient)
  if ((intStatus & bit(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    fifoCount = mpu.getFIFOCount();  // will be zero after reset no need to ask
    Serial.println(fifoCount);
    DEBUG_PRINT("areal\t");
    DEBUG_PRINT(aaReal.x);
    DEBUG_PRINT("\t");
    DEBUG_PRINT(aaReal.y);
    DEBUG_PRINT("\t");
    DEBUG_PRINTLN(aaReal.z);
    Serial.println(F("FIFO overflow!"));
    // Move out of function so we do not process bad samples
    return;
    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (intStatus & bit(MPU6050_INTERRUPT_DMP_INT_BIT)) {
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
