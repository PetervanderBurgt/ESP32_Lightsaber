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

// This is set by the configmenu
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
  DEBUG_PRINTLN("Testing MPU6050 connection...");
  if (!mpu.testConnection()) {
    DEBUG_PRINTLN("MPU6050 connection failed");
    while (true)
      ;
  }

  DEBUG_PRINTLN("MPU6050 connection successful");

  /* Supply your gyro offsets here, read with MPU zero example */
  mpu.setXAccelOffset(X_ACCEL_OFFSET);
  mpu.setYAccelOffset(Y_ACCEL_OFFSET);
  mpu.setZAccelOffset(Z_ACCEL_OFFSET);
  mpu.setXGyroOffset(X_GYRO_OFFSET);
  mpu.setYGyroOffset(Y_GYRO_OFFSET);
  mpu.setZGyroOffset(Z_GYRO_OFFSET);

  // Set Digital Low and High pass filter
  mpu.setDHPFMode(4);

  // Motion interrupt configuration (CLASH)
  mpu.setMotionDetectionThreshold(CLASH_THRESHOLD);
  mpu.setMotionDetectionDuration(CLASH_DURATION);
  mpu.setIntMotionEnabled(true);

  // Latch interrupt until cleared
  mpu.setInterruptLatch(true);
  mpu.setInterruptMode(false);

  attachInterrupt(digitalPinToInterrupt(MPU_INTERRUPT), DMPDataReady, RISING);

  DEBUG_PRINTLN("MPU Ready (No DMP, No FIFO)");
}

void MovementDetection::MPUCode() {
  DEBUG_PRINT("MPUTask running on core ");
  DEBUG_PRINTLN(xPortGetCoreID());
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS((1000 / MPU_HZ));
  initMPU();

  mpu_ready = true;
  

  // Reset any accidental interrupts
  (void)mpu.getIntStatus();
  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    if (lightsaber_on_state == lightsaber_on_ignition) {
      // When turning on clear old interrupt statuses
      (void)mpu.getIntStatus();
      MPUIntStatus = 0x0;
    }
    readMPUData();

    // Only execute swing logic when saber ON
    if (global_state == lightsaber_on) {
      // Always read accel for swing detection
      // If motion interrupt triggered (CLASH)
      if (MPUDataReady) {
        MPUDataReady = false;
        MPUIntStatus = mpu.getIntStatus();  // clears interrupt
      }
      // Let on and off animation do its thing
      if ((lightsaber_on_state != lightsaber_on_ignition) && (lightsaber_on_state != lightsaber_on_retraction)) {
        handleClash();
        handleSwing();
      }
    }
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void MovementDetection::handleClash() {
  bool clashInt = ((MPUIntStatus >> 6) & 0x1) != 0;  // Only check the motion bit
  MPUIntStatus = 0x0;

  // This is only done when the motion interrupt pin of the interrupt status is set, which can be configured by
  // setMotionDetectionThreshold and setMotionDetectionDuration
  if (clashInt && !clashTriggered && !blastTriggered && !swingTriggered && !lockupTriggered) {

    if (lockup_enabled) {
      lockup_enabled = false;

      lockupTriggered = true;
      startLockupMillis = millis();
      lightsaber_on_state = lightsaber_on_bladelockup;
    } else {
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
  float magnitude = sqrt(ax * ax + ay * ay + az * az) / NormFactor;
  // float motion = abs(magnitude - 16384);  // remove 1g

  const uint16_t upperThreshold = SWING_SENSITIVITY_MAX_THRESHOLD;
  bool motionInt = (magnitude > swingSensitivity) && (magnitude < upperThreshold);
  if (motionInt && !clashTriggered && !blastTriggered && !swingTriggered && !lockupTriggered) {

    if (blaster_enabled) {
      blaster_enabled = false;

      blastTriggered = true;
      startBlastMillis = millis();
      effectLeds = random(effectLedsLength, NUM_LEDS);

      lightsaber_on_state = lightsaber_on_blasterdeflect;
    } else {
      /* Display real acceleration, adjusted to remove gravity */

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
  mpu.getAcceleration(&ax, &ay, &az);

  gx = gravityAlpha * gx + (1.0 - gravityAlpha) * ax;
  gy = gravityAlpha * gy + (1.0 - gravityAlpha) * ay;
  gz = gravityAlpha * gz + (1.0 - gravityAlpha) * az;

  // Subtract gravity → real acceleration
  ax -= gx;
  ay -= gy;
  az -= gz;
  float magnitude = sqrt(ax * ax + ay * ay + az * az) / NormFactor;
}
