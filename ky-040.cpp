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
 */

#include "ky-040.h"

/*
 * The interrupt parameters. This library currently only supports the two
 * interrupts on pins 2 and 3.
 */
volatile encoderParams * ky040 :: params_0;
volatile encoderParams * ky040 :: params_1;
uint8_t ky040 :: dtPin_0 = 0;		// initalize to invalid pins
uint8_t ky040 :: dtPin_1 = 0;		// initalize to invalid pins

/*
 * Encoder object creation. We don't attach the interrupt procedure here
 * because no rotaries have been added. No some sanity checks on valid
 * parameters.
 */
ky040 :: ky040 ( uint8_t interruptClkPin, uint8_t dtPin, uint8_t switchPin, uint8_t maxRotarys ) {
	numRotarys = 0;
	this->maxRotarys = 0;
	if ( (interruptClkPin != 2) && (interruptClkPin != 3) )
		return;			// Wrong interrupt pins, error
	params = (encoderParams *) malloc(maxRotarys * sizeof(encoderParams));
	if ( params == 0 )	// Major malloc error
		return;
	this->maxRotarys = maxRotarys;
	// No checks on the switch or dt pins - ???
	swPin = switchPin;
	pinMode(swPin,INPUT_PULLUP);
	clkPin = interruptClkPin;
	pinMode(clkPin,INPUT_PULLUP);
	this->dtPin = dtPin;
	pinMode(dtPin,INPUT_PULLUP);
}

/*
 * Add a new rotary using the passed parameters. Checks are made to ensure
 * the memory for the rotary has been allocated, the id is valid and it
 * has not already been used. The added rotary becomes the current one.
 * Once the rotary is initialized, if it is the first rotary then the actual
 * interrupt procedure is attached.
 */
bool ky040 :: AddRotaryCounter(uint8_t id, int16_t currentVal, int16_t minVal,
							   int16_t maxVal, int16_t inc, bool rollOver ) {
	if ( (id != CURRENT_ID) && !GetParamsFromID(id) && (numRotarys < maxRotarys) ) {
		params[numRotarys].id = id;
		params[numRotarys].currentVal = currentVal;
		params[numRotarys].minVal = minVal;
		params[numRotarys].maxVal = maxVal;
		params[numRotarys].inc = inc;
		params[numRotarys].rollOver = rollOver;
		params[numRotarys].changed = false;
		numRotarys++;
		SetRotary(id);		// Make this one active
		// Has the interrupt procedure been attached yet?
		if ( (clkPin == 2) && (ky040::dtPin_1 == 0) ) {
			ky040::dtPin_1 = dtPin;
			attachInterrupt(1, ky040::RotaryClkInterruptOn_1, FALLING);
		}
		else if ( (clkPin == 3) && (ky040::dtPin_0 == 0) ) {
			ky040::dtPin_0 = dtPin;
			attachInterrupt(0, ky040::RotaryClkInterruptOn_0, FALLING);
		}
		return true;
	}
	return false;		// Add failed.
}

/*
 * Make the requested rotary id the current one.
 */
bool ky040 :: SetRotary ( uint8_t id ) {
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

/*
 * Has the requested rotary id value changed? Reset the changed status
 * once read.
 */
bool ky040 :: HasRotaryValueChanged ( uint8_t id ) {
	bool changed = false;
	if ( GetParamsFromID ( id ) ) {
		noInterrupts();
		changed = currentRotaryParams->changed;
		currentRotaryParams->changed = false;
		interrupts();
	}
	return changed;
}

/*
 * Set the maximum value for the requested rotary. TODO: have a generic
 * fuction to changed all of the parameters on a rotary.
 */
void ky040 :: SetMaxValueOnRotary ( int16_t maxVal, uint8_t id ) {
	if ( GetParamsFromID ( id ) ) {
		noInterrupts();
		currentRotaryParams->maxVal = maxVal;
		currentRotaryParams->currentVal = currentRotaryParams->minVal;
		interrupts();
	}
}

/*
 * Force the requested rotary id to report it has changed.
 */
void ky040 :: SetChanged ( uint8_t id ) {
	if ( GetParamsFromID ( id ) )
		currentRotaryParams->changed = true;
}

/*
 * Return the current value for the requested rotary id
 */
int16_t ky040 :: GetRotaryValue ( uint8_t id ) {
	if ( GetParamsFromID ( id ) )
		return currentRotaryParams->currentVal;
	return 0;	// Not a valid value, but one is needed.
}

/*
 * Return True if the requested rotary id is the currently active one.
 */
bool ky040 :: IsActive ( uint8_t id ) {
	return id == currentID;
}

/*
 * Granted, this is not the best solution for debouncing a switch, but it
 * works OK.
 */
bool ky040 :: SwitchPressed ( void ) {
	if ( digitalRead(swPin) == false ) {
		// Button pressed. Wait for button release.
		while ( digitalRead(swPin) == false ) ;
		// We may get here and its still bouncing.
		delay(100);
		return true;
	}
	return false;
}

/*-private function 
 * Find the parameter block for the requested id.
 */
bool ky040 :: GetParamsFromID ( uint8_t id ) {
	if ( id == CURRENT_ID ) id = currentID;
	for ( uint8_t i = 0; i < numRotarys; i++ ) {
		if ( id == params[i].id )	{
			currentRotaryParams = &params[i];
			return true;
		}
	}
	return false;
}

/*----------------------------------------------------------------------
 * The actual interrupt procedure for each of the two possible encoders
 * that may be connected to the Arduino.
 */
void ky040 :: RotaryClkInterruptOn_0 ( void ) {
	ky040::UpdateRotaryCount(ky040::dtPin_0,ky040::params_0);
}

void ky040 :: RotaryClkInterruptOn_1 ( void ) {
	ky040::UpdateRotaryCount(ky040::dtPin_1,ky040::params_1);
}

/*
 * Generic procedure to increment/decrement the rotary counter
 */
void ky040 :: UpdateRotaryCount ( uint8_t pin, volatile encoderParams * params ) {
	params->changed = true;
	if ( digitalRead(pin) == HIGH ) {
		params->currentVal += params->inc;
		if ( params->currentVal > params->maxVal ) {
			params->currentVal = params->rollOver ?
				params->minVal : params->maxVal;
		}
	}
	else {
		params->currentVal -= params->inc;
		if ( params->currentVal < params->minVal ) {
			params->currentVal = params->rollOver ?
				params->maxVal : params->minVal;
		}
	}
}
//----------------------------------------------------------------------
