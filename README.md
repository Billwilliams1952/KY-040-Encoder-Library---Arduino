# KY-040-Encoder-Library---Arduino

## Synopsis
Arduino library for the KY-040 Encoder. 

This library allows the user to connect up to two encoders using interrupts 0 (pin D3) and interrupt 1 (pin D2). The user may then add rotaries that will each keep track of the encoder movement while active. Each rotary is initialized with the rotary ID (a byte value between 0 - 254), an initial integer value, and integer values for minimum, maximum, and increment. Also a boolean rollover flag controls whether the current rotary value wraps from min to max or max to min. If more encoders are needed, the source code provides comments showing what edits would be needed.

Note, memory for the total number of rotaries for an encoder is allocated during object creation. If you attempt to add a rotary using **AddRotaryCounter** that exceeds the limit set during object creation, the function will return **false**.

The KY-040 encoder library uses interrupts on the clock line. Because of switch bounces, a 470 nF (0.47 uF) capacitor is required on each encoder clock pin connected from the Arduino pin to ground.

<center>**Switch Bounce on Clock (interrupt) line (top trace)**</center>

![alt tag](https://cloud.githubusercontent.com/assets/3778024/20508566/0f773998-b028-11e6-93be-419c9a08014c.png)

**0.47 uF capacitor connected between CLK line and ground** 

![alt tag](https://cloud.githubusercontent.com/assets/3778024/20508570/1a946d00-b028-11e6-8b36-ff34b561c11a.png)

## Installation

Download the ZIP file and extract into your **sketchbook/libraries** directory. Exit the Arduino program (if open) and restart it to see the KY-040 library with its sketch examples.

## API Reference

	ky040 ( uint8_t interruptClkPin, uint8_t dtPin, uint8_t switchPin,
			uint8_t maxRotarys = 1 );
	
	bool AddRotaryCounter ( uint8_t id, int16_t currentVal, int16_t minVal,
			int16_t maxVal, int16_t inc = 1, bool rollOver = true );
	
	bool SetRotary ( uint8_t id );

	bool HasRotaryValueChanged ( uint8_t id = CURRENT_ID );

	void SetChanged ( uint8_t id = CURRENT_ID );
	
	int16_t GetRotaryValue ( uint8_t id = CURRENT_ID );

	void SetMaxValueOnRotary ( int16_t maxVal, uint8_t id = CURRENT_ID );

	bool IsActive ( uint8_t id );

	bool SwitchPressed ( void );

This program uses the Arduino API (**Arduino.h**) and **stdlib.h**; no other special libraries are required. It has been tested on the Arduino Micro.

## Tests

Use the **EncoderTest** example program to verify correct operation.  Be sure to connect a 0.47 uF capacitor from the encoder **CLK** pin to ground.  Also, a sketch using multiple rotaries for one encoder may be found under https://github.com/Billwilliams1952/Arduino-Controlled-Fan-Tester.

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.
