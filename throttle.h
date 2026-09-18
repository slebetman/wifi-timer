#ifndef THROTTLE
#define THROTTLE
#include "platform.h"

#if defined(WEMOS_D1_MINI)
	#include <Servo.h>
#elif defined(ESP32_C3)
	#include <ESP32Servo.h>
#endif

#define SERVO_MIN   1000
#define SERVO_MAX   2000

#define THROTTLE_OFF SERVO_MIN
#define THROTTLE_FULL SERVO_MAX

Servo throttle;

void initThrottle(int pin)
{
	throttle.attach(ESC_PIN);
}

// Calling .writeMicroseconds too frequently corrupts millis()
// This is for rate limiting:
byte throttleCount = 0;
int throttleValue;
#define SERVO_BANDGAP 20
#define SERVO_RATE 50
void setThrottle(int value)
{
	throttleCount++;
	if (
		std::abs(value - throttleValue) > SERVO_BANDGAP ||
		throttleCount > SERVO_RATE
	)
	{
		throttleValue = value;
		throttleCount = 0;
		throttle.writeMicroseconds(value);
	}
}

#endif