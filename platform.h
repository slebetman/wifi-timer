#ifndef PLATFORM_DEFS
#define PLATFORM_DEFS

#if defined(ARDUINO_ESP8266_WEMOS_D1MINI)
	#define WEMOS_D1_MINI
	#define BUTTON_PIN  D1
	#define ESC_PIN     D2
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
	#define ESP32_C3
	#define BUTTON_PIN  0
	#define ESC_PIN     1
	#define LED_BUILTIN 8
#endif

#endif