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
#define NUM_LEDS 100
#define BRIGHTNESS 100

#define CLASH_THRESHOLD 10 // 1mg/LSB  milli G, 1G being earths force
#define CLASH_DURATION 2

#define LEDS_HZ 120
#define BUTTONS_HZ 60
#define MPU_HZ 60
#define DFPLAYER_HZ 120

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