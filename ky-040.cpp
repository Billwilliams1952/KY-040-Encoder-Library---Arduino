/*
 * ky-040.c
 *
 * Copyright 2016 Bill Williams <wlwilliams1952@gmail.com, github/Billwilliams1952>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "ky-040.h"

volatile encoderParams * ky040 :: params_0;
volatile encoderParams * ky040 :: params_1;
byte ky040 :: dtPin_0;
byte ky040 :: dtPin_1;

ky040 :: ky040 ( byte interruptClkPin, byte dtPin, byte switchPin, byte maxRotarys ) {
	numRotarys = 0;
	params = (encoderParams *) malloc(maxRotarys * sizeof(encoderParams));
	if ( params == 0 )	{	// Major error, don't attach any interrupts
		this->maxRotarys = 0;
		return;
	}
	this->maxRotarys = maxRotarys;
	swPin = switchPin;
	pinMode(swPin,INPUT_PULLUP);
	clkPin = interruptClkPin;
	pinMode(clkPin,INPUT_PULLUP);
	pinMode(dtPin,INPUT_PULLUP);

	if ( interruptClkPin == 3 ) {		// Interrupt 0
		ky040::params_0 = params;
		ky040::dtPin_0 = dtPin;
		attachInterrupt(0, ky040::RotaryClkInterruptOn_0, FALLING);
	}
	else {	// Assume on 2 - if not then no harm, no foul, no interrupts
		ky040::params_1 = params;
		ky040::dtPin_1 = dtPin;
		attachInterrupt(1, ky040::RotaryClkInterruptOn_1, FALLING);
	}
}

bool ky040 :: AddRotaryCounter(byte id, int currentVal, int minVal,
							   int maxVal, int inc, bool rollOver ) {
	if ( (id != CURRENT_ID) && (numRotarys < maxRotarys) ) {
		params[numRotarys].id = id;
		params[numRotarys].currentVal = currentVal;
		params[numRotarys].minVal = minVal;
		params[numRotarys].maxVal = maxVal;
		params[numRotarys].inc = inc;
		params[numRotarys].rollOver = rollOver;
		params[numRotarys].changed = false;
		currentID = id;
		numRotarys++;
		return true;
	}
	return false;
}

bool ky040 :: SetRotary ( byte id ) {
	if ( GetParamsFromID ( id ) ) {
		/* Assign correct structure to the appropriate interrupt */
		currentID = id;
		noInterrupts();
		if ( clkPin == 3 )
			ky040::params_0 = currentRotaryParams;
		else
			ky040::params_1 = currentRotaryParams;
		interrupts();
		return true;
	}
	return false;
}

bool ky040 :: HasRotaryValueChanged ( byte id ) {
	bool changed = false;
	if ( GetParamsFromID ( id ) ) {
		changed = currentRotaryParams->changed;
		currentRotaryParams->changed = false;
	}
	return changed;
}

void ky040 :: SetMaxValueOnRotary ( int maxVal, byte id ) {
	if ( GetParamsFromID ( id ) ) {
		noInterrupts();
		currentRotaryParams->maxVal = maxVal;
		currentRotaryParams->currentVal = currentRotaryParams->minVal;
		interrupts();
	}
}

void ky040 :: SetChanged ( byte id ) {
	if ( GetParamsFromID ( id ) ) {
		currentRotaryParams->changed = true;
	}
}

int ky040 :: GetRotaryValue ( byte id ) {
	if ( GetParamsFromID ( id ) )
		return currentRotaryParams->currentVal;
	return 0;
}

bool ky040 :: IsActive ( byte id ) {
	return id == currentID;
}

bool ky040 :: SwitchPressed ( void ) {
	if ( digitalRead(swPin) == false ) {
		while ( digitalRead(swPin) == false ) ;
		delay(100);
		return true;
	}
	return false;
}

bool ky040 :: GetParamsFromID ( byte id ) {
	if ( id == CURRENT_ID ) id = currentID;
	for ( byte i = 0; i < numRotarys; i++ ) {
		if ( id == params[i].id )	{
			currentRotaryParams = &params[i];
			return true;
		}
	}
	return false;
}

void ky040 :: RotaryClkInterruptOn_0 ( void ) {
	ky040::UpdateRotaryCount(ky040::dtPin_0,ky040::params_0);
}

void ky040 :: RotaryClkInterruptOn_1 ( void ) {
	ky040::UpdateRotaryCount(ky040::dtPin_1,ky040::params_1);
}

void ky040 :: UpdateRotaryCount ( byte pin, volatile encoderParams * params ) {
	params->changed = true;
	if ( digitalRead(pin) == HIGH ) {
		params->currentVal += params->inc;
		if ( params->currentVal > params->maxVal ) {
			if ( params->rollOver )
				params->currentVal = params->minVal;
			else
				params->currentVal = params->maxVal;
		}
	}
	else {
		params->currentVal -= params->inc;
		if ( params->currentVal < params->minVal ) {
			if ( params->rollOver )
				params->currentVal = params->maxVal;
			else
				params->currentVal = params->minVal;
		}
	}
}
