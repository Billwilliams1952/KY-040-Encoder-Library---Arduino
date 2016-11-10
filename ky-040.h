
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
	int id;
	int currentVal;
	int minVal;
	int maxVal;
	int inc;
	bool rollOver, changed;
} encoderParams;
	
class ky040 {

public:
	
	ky040 ( byte interruptClkPin, byte dtPin, byte switchPin, byte maxRotarys = 1 );
	
	bool AddRotaryCounter(byte id, int currentVal, int minVal, int maxVal, 
						  int inc = 1, bool rollOver = true);
	
	bool SetRotary ( byte id );

	bool HasRotaryValueChanged ( byte id = CURRENT_ID );

	void SetChanged ( byte id = CURRENT_ID );
	
	int GetRotaryValue ( byte id = CURRENT_ID );

	void SetMaxValueOnRotary ( int maxVal, byte id = CURRENT_ID );

	bool IsActive ( byte id );

	bool SwitchPressed ( void );
	
private:
	
	byte numRotarys,
		clkPin,
		dtPin,
		swPin,
		maxRotarys;
	byte currentID;
		
	encoderParams *params;
	volatile encoderParams *currentRotaryParams;
	bool GetParamsFromID ( byte id );
	
	static volatile encoderParams * params_0, *params_1;
	static byte dtPin_0;
	static byte dtPin_1;
	static void RotaryClkInterruptOn_0 ( void );
	static void RotaryClkInterruptOn_1 ( void );
	static void UpdateRotaryCount ( byte pin, volatile encoderParams * params );
	
};

#endif
