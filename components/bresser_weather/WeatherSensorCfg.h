///////////////////////////////////////////////////////////////////////////////////////////////////
// WeatherSensorCfg.h
//
// Bresser 5-in-1/6-in-1/7-in-1 868 MHz Weather Sensor Radio Receiver
// based on CC1101, SX1276/RFM95W, SX1262 or LR1121 and ESP32/ESP8266
//
// https://github.com/matthias-bs/BresserWeatherSensorReceiver
//
// NOTE: Application/hardware specific configurations should be made in this file!
//
// created: 05/2022
//
//
// MIT License
//
// Copyright (c) 2025 Matthias Prinke
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// This version lives here to override the one in the library.
// It is copied to the library folder during build by pre_build.py
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(WEATHER_SENSOR_CFG_H)
#define WEATHER_SENSOR_CFG_H

#include <Arduino.h>

// ------------------------------------------------------------------------------------------------
// --- Weather Sensors ---
// ------------------------------------------------------------------------------------------------
#define MAX_SENSORS_DEFAULT 1       // Maximum number of sensors to be received

// List of sensor IDs to be excluded - can be empty
//#define SENSOR_IDS_EXC { 0x792882A2 }
#define SENSOR_IDS_EXC { 0x792882A2 }

// List of sensor IDs to be included - if empty, handle all available sensors
#define SENSOR_IDS_INC { }
//#define SENSOR_IDS_INC { 0x83750871 }

// Maximum number of sensor IDs in include/exclude list
#define MAX_SENSOR_IDS 12

// Disable data type which will not be used to save RAM
#define WIND_DATA_FLOATINGPOINT
#define WIND_DATA_FIXEDPOINT

// Select appropriate sensor message format(s)
// Comment out unused decoders to save operation time/power
#define BRESSER_5_IN_1
#define BRESSER_6_IN_1
#define BRESSER_7_IN_1
#define BRESSER_LIGHTNING
#define BRESSER_LEAKAGE


// ------------------------------------------------------------------------------------------------
// --- Rain Gauge / Lightning sensor data retention during deep sleep ---
// ------------------------------------------------------------------------------------------------

#if !defined(INSIDE_UNITTEST)
    #if defined(ESP32)
        // Option: Comment out to save data in RTC RAM
        // N.B.:
        // ESP8266 has RTC RAM, too, but access is different from ESP32
        // and currently not implemented here
        #define RAINGAUGE_USE_PREFS
        #define LIGHTNING_USE_PREFS
    #else
        // Using Preferences is mandatory on other architectures (e.g. RP2040)
        #define RAINGAUGE_USE_PREFS
        #define LIGHTNING_USE_PREFS
    #endif
#endif

#if defined(USE_CC1101)
#define RADIO_CHIP CC1101
#elif defined(USE_SX1276)
#define RADIO_CHIP SX1276
#elif defined(USE_SX1262)
#define RADIO_CHIP SX1262
#elif defined(USE_LR1121)
#define RADIO_CHIP LR1121
#else
#pragma message("No radio chip selected!")
#endif


// ------------------------------------------------------------------------------------------------
// --- Debug Logging Output ---
// ------------------------------------------------------------------------------------------------
// - ESP32:
//   CORE_DEBUG_LEVEL is set in Adruino IDE:
//   Tools->Core Debug Level: "<None>|<Error>|<Warning>|<Info>|<Debug>|<Verbose>"
//   https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-log.h
//
// - ESP8266:
//   DEBUG_ESP_PORT is set in Arduino IDE:
//   Tools->Debug port: "<None>|<Serial>|<Serial1>"
//
// - RP2040:
//   DEBUG_RP2040_PORT is set in Arduino IDE:
//   Tools->Debug port: "<Disabled>|<Serial>|<Serial1>|<Serial2>"
//
//   Replacement for
//   https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-log.h
//   on ESP8266 and RP2040:
#if defined(ESP8266) || defined(ARDUINO_ARCH_RP2040)
    #define ARDUHAL_LOG_LEVEL_NONE      0
    #define ARDUHAL_LOG_LEVEL_ERROR     1
    #define ARDUHAL_LOG_LEVEL_WARN      2
    #define ARDUHAL_LOG_LEVEL_INFO      3
    #define ARDUHAL_LOG_LEVEL_DEBUG     4
    #define ARDUHAL_LOG_LEVEL_VERBOSE   5

    #if defined(ARDUINO_ARCH_RP2040) && defined(DEBUG_RP2040_PORT)
        #define DEBUG_PORT DEBUG_RP2040_PORT
    #elif defined(DEBUG_ESP_PORT)
        #define DEBUG_PORT DEBUG_ESP_PORT
    #endif
    
    // Set desired level here if not defined elsewhere!
    #if !defined(CORE_DEBUG_LEVEL)
        #define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_INFO
    #endif

    #if defined(DEBUG_PORT) && CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_NONE
        #define log_e(...) { DEBUG_PORT.printf("%s(), l.%d: ",__func__, __LINE__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
     #else
        #define log_e(...) {}
     #endif
    #if defined(DEBUG_PORT) && CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_ERROR
        #define log_w(...) { DEBUG_PORT.printf("%s(), l.%d: ", __func__, __LINE__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
     #else
        #define log_w(...) {}
     #endif
    #if defined(DEBUG_PORT) && CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_WARN
        #define log_i(...) { DEBUG_PORT.printf("%s(), l.%d: ", __func__, __LINE__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
     #else
        #define log_i(...) {}
     #endif
    #if defined(DEBUG_PORT) && CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_INFO
        #define log_d(...) { DEBUG_PORT.printf("%s(), l.%d: ", __func__, __LINE__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
     #else
        #define log_d(...) {}
     #endif
    #if defined(DEBUG_PORT) && CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_DEBUG
        #define log_v(...) { DEBUG_PORT.printf("%s(), l.%d: ", __func__, __LINE__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
     #else
        #define log_v(...) {}
     #endif

#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define RECEIVER_CHIP "[" STR(RADIO_CHIP) "]"
#pragma message("Receiver chip: " RECEIVER_CHIP)
#pragma message("Pin config: RST->" STR(PIN_RECEIVER_RST) ", CS->" STR(PIN_RECEIVER_CS) ", GD0/G0/IRQ->" STR(PIN_RECEIVER_IRQ) ", GDO2/G1/GPIO->" STR(PIN_RECEIVER_GPIO) )

#endif
