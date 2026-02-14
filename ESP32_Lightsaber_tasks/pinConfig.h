#ifndef PINCONFIG_H
#define PINCONFIG_H

// Pin Definitions
#define RX_DFPLAYER 16
#define TX_DFPLAYER 17

#define MAIN_BUTTON 32
#define MAIN_RED 23
#define MAIN_GREEN 19
#define MAIN_BLUE 18

#define SECOND_BUTTON 25
#define SECOND_RED 13
#define SECOND_GREEN 33
#define SECOND_BLUE 26

#define MPU_INTERRUPT 4

#define LED_OUTPUT 5
#define CRYSTAL_LED_OUTPUT 27
#define NUM_LEDS_CRYSTAL 1
#define NUM_LEDS 100
#define TIPMELT_LEDS 10
#define BRIGHTNESS 100

#define X_ACCEL_OFFSET -6794
#define Y_ACCEL_OFFSET -1362
#define Z_ACCEL_OFFSET 410
#define X_GYRO_OFFSET 218
#define Y_GYRO_OFFSET -22
#define Z_GYRO_OFFSET 54

#define SWING_SENSITIVITY_INITIAL 1760
#define CLASH_THRESHOLD 33 // 1mg/LSB  milli G, 1G being earths force
#define CLASH_DURATION 2

#define BLADE_IGNITION_MS 800
#define BLADE_RETRACTION_MS 800
#define CLASH_FX_DURATION 750
#define LOCKUP_FX_DURATION 4000
#define BLASTER_FX_DURATION 500
#define SWING_FX_DURATION 300
#define TIPMELT_FX_DURATION 4000

#define LEDS_HZ 60
#define BUTTONS_HZ 30
#define MPU_HZ 60
#define DFPLAYER_HZ 20

#define LED_TASK_STACK_SIZE 6144
#define BUTTONS_TASK_STACK_SIZE 2048
#define MPU_TASK_STACK_SIZE 6144
#define DFPLAYER_TASK_STACK_SIZE 4096

#define LED_TASK_PRIORITY 1 // 3
#define BUTTONS_TASK_PRIORITY 1 // 1
#define MPU_TASK_PRIORITY 1 // 4
#define DFPLAYER_TASK_PRIORITY 1 // 2


#define DISABLE_PRINT 0

// Define a macro for Serial.println that only works if DISABLE_PRINT is not set to 1
#if defined(DISABLE_PRINT) && DISABLE_PRINT == 1
#define DEBUG_PRINT(x)    // Empty definition, effectively disables printing
#define DEBUG_PRINTLN(x)  // Empty definition, effectively disables printing
#else
#define DEBUG_PRINT(x) Serial.print(x)      // Enable Serial.println
#define DEBUG_PRINTLN(x) Serial.println(x)  // Enable Serial.println
#endif

#endif