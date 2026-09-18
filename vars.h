#include <EEPROM.h>

extern struct Vars vars;

#ifndef VARS_H
#define VARS_H

const uint32_t MAGIC = 0xabcd0011;

struct Vars
{
	uint32_t magic;
	byte max;
	float startDelay;
	float rampUp;
	float cruise;
	float rampDown;
};

Vars vars;

void initVars()
{
	EEPROM.begin(sizeof(vars));
}

void saveVars()
{
	EEPROM.put(0, vars);
	EEPROM.commit();
}

Vars *readVars()
{
	EEPROM.get(0, vars);

	if (vars.magic != MAGIC)
	{
		// defaults:
		vars.magic = MAGIC;
		vars.max = 255;
		vars.startDelay = 0;
		vars.rampUp = 0.5;
		vars.cruise = 10;
		vars.rampDown = 1.5;

		saveVars();
	}

	return &vars;
}

#endif