/*
 * ky-040.c
 *
 * Copyright 2016 Bill Williams <wlwilliams1952@gmail.com, github/Billwilliams1952>
 *
 * Library to interface with the KY-040 rotary encoder. This library
 * supports the Arduino Uno, Pro, Mini, Nano, Micro, Leonardo, and Mega.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#include "ky-040.h"

/*
 * The interrupt parameters. Default is the two interrupts on pins 2 and 3
 * #defines are used to identify the unique Arduino board.
 * Arduino Uno, Nano, Duemilanove, Mini only have two interrupt pins
 * Basically anything based off the ATmega328P or ATmega168
 */

volatile encoderParams * ky040 :: params_2;
volatile encoderParams * ky040 :: params_3;

uint8_t ky040 :: dtPin_2 = 0;		// initalize to invalid pins
uint8_t ky040 :: dtPin_3 = 0;		// initalize to invalid pins

/*
 * Arduino Mega
 */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	// Added: 18 (interrupt 5), 19 (interrupt 4), 20 (interrupt 3),
	// and 21 (interrupt 2).
	volatile encoderParams * ky040 :: params_18;
	volatile encoderParams * ky040 :: params_19;
	volatile encoderParams * ky040 :: params_20;
	volatile encoderParams * ky040 :: params_21;
	uint8_t ky040 :: dtPin_18 = 0;		// initalize to invalid pins
	uint8_t ky040 :: dtPin_19 = 0;		// initalize to invalid pins
	uint8_t ky040 :: dtPin_20 = 0;		// initalize to invalid pins
	uint8_t ky040 :: dtPin_21 = 0;		// initalize to invalid pins
#endif

/*
 * Arduino Leonardo, Micro
 */
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
	// Added: 0 (interrupt 2), 1 (interrupt 3) and 7 (interrupt 4)
	volatile encoderParams * ky040 :: params_0;
	volatile encoderParams * ky040 :: params_1;
	volatile encoderParams * ky040 :: params_7;
	uint8_t ky040 :: dtPin_0 = 255;		// initalize to invalid pins
	uint8_t ky040 :: dtPin_1 = 0;		// initalize to invalid pins
	uint8_t ky040 :: dtPin_7 = 0;		// initalize to invalid pins
#endif

/*
 * Encoder object creation. We don't attach the interrupt procedure here
 * because no rotaries have been added. Some sanity checks on valid
 * parameters.
 *
 * I decided to force the user to define the number of rotaries at object
 * creation to ensure there is enough memory. I didn't want to get into
 * malloc's, and realloc's later. Does this fix all problems? Can the
 * user try to create rotaries later? No and Yes.
 */
ky040 :: ky040 ( uint8_t interruptClkPin, uint8_t dtPin, uint8_t switchPin,
				 uint8_t maxRotarys ) {
	/* Checks for valid interrupt pin */
	params = 0;		// null until valid clock pin specified and then malloc'd
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
	if ( (interruptClkPin != 2) && (interruptClkPin != 3) )
		return;			// Wrong interrupt pins, error
#endif
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	if ( (interruptClkPin != 2) && (interruptClkPin != 3) &&
		 (interruptClkPin != 18) && (interruptClkPin != 19) &&
		 (interruptClkPin != 20) && (interruptClkPin != 21) )
		return;			// Wrong interrupt pins, error
#endif
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
	if ( (interruptClkPin != 2) && (interruptClkPin != 3) &&
		 (interruptClkPin != 0) && (interruptClkPin != 1) &&
		 (interruptClkPin != 7) )
		return;			// Wrong interrupt pins, error
#endif
	params = (encoderParams *) malloc(maxRotarys * sizeof(encoderParams));
	if ( params == 0 )	// Major malloc error
		return;
	maxRotaries = maxRotarys;
	// TODO: Check on validity of pins?
	swPin = switchPin;
	pinMode(swPin,INPUT_PULLUP);	// TODO: Do we need pullups? CHECK
	clkPin = interruptClkPin;
	pinMode(clkPin,INPUT_PULLUP);	// TODO: Do we need pullups? CHECK
	this->dtPin = dtPin;
	pinMode(dtPin,INPUT_PULLUP);	// TODO: Do we need pullups? CHECK
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
	if ( (params != 0) && (id != CURRENT_ID) && !GetParamsFromID(id) && (numRotaries < maxRotaries) ) {
		params[numRotaries].id = id;
		params[numRotaries].currentVal = currentVal;
		params[numRotaries].minVal = minVal;
		params[numRotaries].maxVal = maxVal;
		params[numRotaries].inc = inc;
		params[numRotaries].rollOver = rollOver;
		params[numRotaries].changed = false;
		numRotaries++;
		SetRotary(id);		// Make this one active
		// Has the interrupt procedure been attached yet?
		if ( (clkPin == 2) && (ky040::dtPin_2 == 0) ) {
			ky040::dtPin_2 = dtPin;
			/* The reason we are using the FALLING edge is because of the
			 * 0.47uF capacitor connected from the interrupt pin to ground.
			 * With pullups enabled, there is a slow risetime (helping with
			 * debounce), while the high to low transition occurs quickly,
			 * into a low impedance - improving interrupt response time.
			 * TODO: Check if defining INPUT_PULLUP is required.
			 */ 
			attachInterrupt(digitalPinToInterrupt(clkPin),
				ky040::RotaryClkInterruptOn_2, FALLING);
		}
		else if ( (clkPin == 3) && (ky040::dtPin_3 == 0) ) {
			ky040::dtPin_3 = dtPin;
			attachInterrupt(digitalPinToInterrupt(clkPin),
				ky040::RotaryClkInterruptOn_3, FALLING);
		}
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		else if ( (clkPin == 18) && (ky040::dtPin_18 == 0) ) {
			ky040::dtPin_18 = dtPin;
			attachInterrupt(digitalPinToInterrupt(clkPin),
				ky040::RotaryClkInterruptOn_18, FALLING);
		}
		else if ( (clkPin == 19) && (ky040::dtPin_19 == 0) ) {
			ky040::dtPin_19 = dtPin;
			attachInterrupt(digitalPinToInterrupt(clkPin),
				ky040::RotaryClkInterruptOn_19, FALLING);
		}
		else if ( (clkPin == 20) && (ky040::dtPin_20 == 0) ) {
			ky040::dtPin_20 = dtPin;
			attachInterrupt(digitalPinToInterrupt(clkPin),
				ky040::RotaryClkInterruptOn_20, FALLING);
		}
		else if ( (clkPin == 21) && (ky040::dtPin_21 == 0) ) {
			ky040::dtPin_21 = dtPin;
			attachInterrupt(digitalPinToInterrupt(clkPin),
				ky040::RotaryClkInterruptOn_21, FALLING);
		}
#endif
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
		else if ( (clkPin == 0) && (ky040::dtPin_0 == 255) ) {
			ky040::dtPin_0 = dtPin;
			attachInterrupt(digitalPinToInterrupt(clkPin),
				ky040::RotaryClkInterruptOn_0, FALLING);
		}
		else if ( (clkPin == 1) && (ky040::dtPin_1 == 0) ) {
			ky040::dtPin_1 = dtPin;
			attachInterrupt(digitalPinToInterrupt(clkPin),
				ky040::RotaryClkInterruptOn_1, FALLING);
		}
		else if ( (clkPin == 7) && (ky040::dtPin_7 == 0) ) {
			ky040::dtPin_7 = dtPin;
			attachInterrupt(digitalPinToInterrupt(clkPin),
				ky040::RotaryClkInterruptOn_7, FALLING);
		}
#endif
		return true;
	}
	return false;		// Add failed.
}

/*
 * Simple call if a zero based rotary is all that is required
 */
bool ky040 :: AddRotaryCounter(uint8_t id, int16_t maxVal, bool rollOver ) {
	return AddRotaryCounter(id, 0, 0, maxVal, 1, rollOver );
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
			ky040::params_3 = currentRotaryParams;
		else if ( clkPin == 2 )
			ky040::params_2 = currentRotaryParams;
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		else if ( clkPin == 18 )
			ky040::params_18 = currentRotaryParams;
		else if ( clkPin == 19 )
			ky040::params_19 = currentRotaryParams;
		else if ( clkPin == 20 )
			ky040::params_20 = currentRotaryParams;
		else if ( clkPin == 21 )
			ky040::params_21 = currentRotaryParams;
#endif
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
	// 3 (interrupt 0), 2 (interrupt 1), 0 (interrupt 2), 1 (interrupt 3) and 7 (interrupt 4)
		else if ( clkPin == 0 )
			ky040::params_0 = currentRotaryParams;
		else if ( clkPin == 1 )
			ky040::params_1 = currentRotaryParams;
		else if ( clkPin == 7 )
			ky040::params_7 = currentRotaryParams;
#endif
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
 * fuction to changed all of the parameters on a rotary. How often
 * would this be needed?
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
 
/* private function 
 * Find the parameter block for the requested id.
 */
bool ky040 :: GetParamsFromID ( uint8_t id ) {
	if ( params != 0 )	{
		if ( id == CURRENT_ID ) id = currentID;
		for ( uint8_t i = 0; i < numRotaries; i++ ) {
			if ( id == params[i].id )	{
				currentRotaryParams = &params[i];
				return true;
			}
		}
	}
	return false;
}

/*----------------------------------------------------------------------
 * The actual interrupt procedure for each of the two possible encoders
 * that may be connected to the Arduino. To add more encoders, declare
 * more interrupt routines and tie then to the appropriate data.
 */
void ky040 :: RotaryClkInterruptOn_3 ( void ) {
	ky040::UpdateRotaryCount(ky040::dtPin_3,ky040::params_3);
}

void ky040 :: RotaryClkInterruptOn_2 ( void ) {
	ky040::UpdateRotaryCount(ky040::dtPin_2,ky040::params_2);
}

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	void ky040 :: RotaryClkInterruptOn_18 ( void ) {
		ky040::UpdateRotaryCount(ky040::dtPin_18,ky040::params_18);
	}

	void ky040 :: RotaryClkInterruptOn_19 ( void ) {
		ky040::UpdateRotaryCount(ky040::dtPin_19,ky040::params_19);
	}

	void ky040 :: RotaryClkInterruptOn_20 ( void ) {
		ky040::UpdateRotaryCount(ky040::dtPin_20,ky040::params_20);
	}

	void ky040 :: RotaryClkInterruptOn_21 ( void ) {
		ky040::UpdateRotaryCount(ky040::dtPin_21,ky040::params_21);
	}
#endif

#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
	void ky040 :: RotaryClkInterruptOn_0 ( void ) {
		ky040::UpdateRotaryCount(ky040::dtPin_0,ky040::params_0);
	}

	void ky040 :: RotaryClkInterruptOn_1 ( void ) {
		ky040::UpdateRotaryCount(ky040::dtPin_1,ky040::params_1);
	}

	void ky040 :: RotaryClkInterruptOn_7 ( void ) {
		ky040::UpdateRotaryCount(ky040::dtPin_7,ky040::params_7);
	}
#endif

/*
 * Generic procedure to increment/decrement the rotary counter
 */
void ky040 :: UpdateRotaryCount ( uint8_t pin, volatile encoderParams * params ) {
	/* TODO: Add an acceleration test. Store time, then at each interrupt
	 * check interval from last interrupt, if smaller than TBD msec,
	 * increase increment (multiply by 2, 4, 8?). How to handle rollover
	 * is one problem that I need to think about.
	 */
	params->changed = true;
	/* TODO: Could use a direct port read here.... save some time.
	 * TODO: Could just increment / decrement a counter. The actual value
	 * could then be calculated in the GetRotaryValue function. This I
	 * have to think about a bit more.
	 */
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
