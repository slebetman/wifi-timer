#include "platform.h"
#include "tick.h"
#include "ramp.h"
#include "delay.h"
#include "button.h"
#include "wifi.h"
#include "web.h"
#include "vars.h"
#include "blink.h"
#include "throttle.h"

#define AP_SSID     "TIMER_00001"

Tick timer;
Ramp rampUp(THROTTLE_OFF, THROTTLE_FULL);
Ramp rampDown(THROTTLE_FULL, THROTTLE_OFF);
Delay esc;
Delay cruise;
Delay startDelay;
Blink escBlink(LED_BUILTIN, 100);
Blink runBlink(LED_BUILTIN, 500);
Button button(BUTTON_PIN);
Button cancelButton(BUTTON_PIN);
Web server(80);
Wifi wifi;

void initialize()
{
	Serial.println("init");
	setThrottle(THROTTLE_OFF);
	button.init();
	cancelButton.init();
	digitalWrite(LED_BUILTIN, HIGH);
}

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(ESC_PIN, OUTPUT);
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	initVars();
	readVars();

	Serial.begin(115200);
	delay(10);

	initThrottle(ESC_PIN);

	int check = digitalRead(BUTTON_PIN);

	// Throttle calibration
	if (check == LOW)
	{
		Serial.println("Calibration");
		setThrottle(THROTTLE_FULL);
		button.init();
		while (1)
		{
			if (timer.tick())
			{
				check = digitalRead(BUTTON_PIN);
				// ESP.wdtFeed();
				if (check == HIGH)
				{
					setThrottle(THROTTLE_OFF);
					break;
				}
			}
		}
	}

	esc.init(2.5);
	initialize();

	wifi.initAP(AP_SSID);
	// wifi.initClient("MY_WIFI", "PASSWORD");
	server.init();
}

byte runState = 0;

void loop()
{
	server.run();

	if (timer.tick())
	{
		if (esc.wait())
		{ // wait for ESC to initialize
			escBlink.blink();
			setThrottle(THROTTLE_OFF);
		}
		else
		{
			if (runState == 0)
			{
				runState = 1;
				escBlink.stop();
			}

			if (button.click())
			{
				if (button.once)
				{
					int throttleMax = THROTTLE_OFF + (((THROTTLE_FULL - THROTTLE_OFF) * vars.max) / 255);

					Serial.println("Click!");

					startDelay.init(vars.startDelay);
					rampUp.setTo(throttleMax);
					rampUp.init(vars.rampUp);
					cruise.init(vars.cruise);
					rampDown.setFrom(throttleMax);
					rampDown.init(vars.rampDown);
				}
				run();
			}
			else
			{
				setThrottle(THROTTLE_OFF);
			}
		}
	}
}

void run()
{
	runBlink.blink();
	if (startDelay.wait())
	{
		setThrottle(THROTTLE_OFF);
		if (cancelButton.click())
		{
			end();
		}
	}
	else if (rampUp.run())
	{
		setThrottle(std::round(rampUp.value));
		if (cancelButton.click())
		{
			end();
		}
	}
	else if (cruise.wait())
	{
		setThrottle(std::round(rampUp.value));
		if (cancelButton.click())
		{
			end();
		}
	}
	else if (rampDown.run())
	{
		setThrottle(std::round(rampDown.value));
		if (cancelButton.click())
		{
			end();
		}
	}
	else
	{
		end();
	}
}

void end()
{
	Serial.println("end");
	runBlink.stop();
	initialize();
}
