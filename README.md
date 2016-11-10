# KY-040-Encoder-Library---Arduino
Arduino library for the KY-040 Encoder. 

This library allows the user to connect up to two encoders using interrupts 0 (pin D3) and interrupt 1 (pin D2). The user may then add rotaries that will each keep track of the encoder movement while active. Each rotary is initialized with the rotary ID (a byte value between 0 - 254), and initial integer values for minimum, maximum, and increment. Also a bollean rollover flag controls whether the current rotary value wraps from min to max or max to min.

Note, memory for the total number of rotarys for an encoder is initialized during object creation.

The KY-040 encoder library uses interrupts on the clock line.  Becuase of switch bounces, a 470 nF (0.47 uF) capacitor is required on each clock pin.

The code has been tested on an Arduino Micro. See the Examples directory.  Also, a sketch using multiple rotarys for one encoder may be found under https://github.com/Billwilliams1952/Arduino-Controlled-Fan-Tester 

