# KY-040-Encoder-Library---Arduino
Arduino library for the KY-040 Encoder. 

This library allows the user to connect up to two encoders using interrupt 0 and interrupt 1. The user may then add rotaries that will each keep track of the encoder movement while active. Each rotary is initialized with the initial vaslue, minimum, maximum, increment, and rollover flag.  

The KY-040 encoder library uses intyerrupts on the clock line.  Becuase of switch bounces, a 470 nF (0.47 uF) cpacitor is required on each clock pin.


