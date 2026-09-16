#ifndef PLATFORM_DEFS
#define PLATFORM_DEFS

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#define WEMOS_D1_MINI
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
#define ESP32_C3
#define LED_BUILTIN 8
#endif

#endif