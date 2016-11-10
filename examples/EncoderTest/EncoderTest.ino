/*
 * EncoderTest.ino
 * 2016 WLWilliams
 * 
 * This sketch demonstrates the use of the KY-040 encoder library.
 * 
 * This program is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version. 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of
 * the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Library originally added November 9, 2016 by Bill Williams
 *
 * This example code is in the public domain.
 * 
 * https://github.com/Billwilliams1952/KY-040-Encoder-Library---Arduino
 */

// Include the library definition
#include <ky-040.h> 

// Define how the encoder is connected to the Arduino
#define ENCODER_CLK          2      // This pin must have a minimum 0.47 uF capacitor
                                    // The current library only allows pins 2 or 3
#define ENCODER_DT          12      // data pin
#define ENCODER_SW          11      // switch pin (active LOW)

#define MAX_ROTARIES         2      // this example define two rotaries for the encoder
#define ROTARY_ID1           0      // Ids can range from 0 to 254, 255 is reserved
#define ROTARY_ID2          10      // Ids can range from 0 to 254, 255 is reserved

/*
 * Create a KY-040 instance. Allow for up to two rotaries
 */
ky040 rot(ENCODER_CLK, ENCODER_DT, ENCODER_SW, MAX_ROTARIES );

void setup() {
    // put your setup code here, to run once:

    // Add the two rotaries defined above.
    // Define a rotary to go from -20 to 20 in increments of 2. Initialize it to 10. Allow it to rollover
    rot.AddRotaryCounter(ROTARY_ID1, 10, -20, 20, 2, true );
    
    // Define a rotary to go from 50 to 100 in increments of 10. Initialize it to 70. no rollover    
    rot.AddRotaryCounter(ROTARY_ID2, 70, 50, 100, 10, false );

    // Make ROTARY_ID2 active (responds to encoder shaft movements)
    rot.SetRotary(ROTARY_ID2);
    rot.SetChanged(ROTARY_ID2); // This way we can force an update the first time through

    Serial.begin(9600);   // write out the values
}

void loop() {
    // put your main code here, to run repeatedly:
  
    // On each encoder switch press, toggle between the two rotaries and print their values.
    // Use the IsActive call
  
    if ( rot.SwitchPressed() ) {
        if ( rot.IsActive(ROTARY_ID1) )
            rot.SetRotary(ROTARY_ID2);
        else
            rot.SetRotary(ROTARY_ID1);
    }

    if ( rot.HasRotaryValueChanged() || rot.HasRotaryValueChanged() ) {
        Serial.print("Encoder 1: "); Serial.println(rot.GetRotaryValue(ROTARY_ID1));
        Serial.print("Encoder 2: "); Serial.println(rot.GetRotaryValue(ROTARY_ID2));  
    }

}
