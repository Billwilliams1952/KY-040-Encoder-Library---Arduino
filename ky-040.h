
/*
 * ky-040.h
 * 
 * Copyright 2016 Bill Williams <wlwilliams1952@gmail.com, github/Billwilliams1952>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
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
#ifndef __KY040__

#define __KY040__

#include "Arduino.h"
#include <stdlib.h>			// malloc call

#define CURRENT_ID		255

typedef struct encoderP {
	uint8_t id;
	int16_t currentVal;
	int16_t minVal;
	int16_t maxVal;
	int16_t inc;
	bool rollOver, changed;
} encoderParams;
	
class ky040 {

public:
	
	ky040 ( uint8_t interruptClkPin, uint8_t dtPin, uint8_t switchPin,
			uint8_t maxRotarys = 1 );
	
	bool AddRotaryCounter(uint8_t id, int16_t currentVal, int16_t minVal,
			int16_t maxVal, int16_t inc = 1, bool rollOver = true);
	
	bool SetRotary ( uint8_t id );

	bool HasRotaryValueChanged ( uint8_t id = CURRENT_ID );

	void SetChanged ( uint8_t id = CURRENT_ID );
	
	int16_t GetRotaryValue ( uint8_t id = CURRENT_ID );

	void SetMaxValueOnRotary ( int16_t maxVal, uint8_t id = CURRENT_ID );

	bool IsActive ( uint8_t id );

	bool SwitchPressed ( void );
	
private:
	
	uint8_t numRotarys,
			clkPin,
			dtPin,
			swPin,
			maxRotarys,
			currentID;
		
	encoderParams *params;
	volatile encoderParams *currentRotaryParams;
	bool GetParamsFromID ( uint8_t id );
	
	static volatile encoderParams * params_0, *params_1;
	static uint8_t dtPin_0;
	static uint8_t dtPin_1;
	static void RotaryClkInterruptOn_0 ( void );
	static void RotaryClkInterruptOn_1 ( void );
	static void UpdateRotaryCount ( uint8_t pin, volatile encoderParams * params );
	
};

#endif
