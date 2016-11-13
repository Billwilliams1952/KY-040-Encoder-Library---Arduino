/*
 * EncoderTest.ino
 * 2016 WLWilliams
 * 
 * This sketch demonstrates the use of the KY-040 encoder library. To test one encoder
 * instead of two, undefine the TWO_ROTARY_TEST
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

#define TWO_ENCODER_TEST            // Two encoders are connected

// Define how the encoder is connected to the Arduino
// First Encoder
#define ENCODER_CLK1         2      // This pin must have a minimum 0.47 uF capacitor
                                    // The current library only allows pins 2 or 3
#define ENCODER_DT1         12      // data pin
#define ENCODER_SW1         11      // switch pin (active LOW)
#define MAX_ROTARIES1        2      // this example define two rotaries for this encoder

// Second Encoder
#define ENCODER_CLK2         3      // This pin must have a minimum 0.47 uF capacitor
                                    // The current library only allows pins 2 or 3     
#define ENCODER_DT2         10      // data pin
#define ENCODER_SW2          9      // switch pin (active LOW)
#define MAX_ROTARIES2        3      // this example define three rotaries for this encoder


#define ROTARY_ID1           0      // Ids can range from 0 to 254, 255 is reserved
#define ROTARY_ID2          10      // Ids can range from 0 to 254, 255 is reserved
#define ROTARY_ID3          11      // Ids can range from 0 to 254, 255 is reserved
/*
 * Create two KY-040 encoders.
 */
ky040 encoder1(ENCODER_CLK1, ENCODER_DT1, ENCODER_SW1, MAX_ROTARIES1 );

#ifdef TWO_ENCODER_TEST
ky040 encoder2(ENCODER_CLK2, ENCODER_DT2, ENCODER_SW2, MAX_ROTARIES2 );
#endif

void setup() {
    // put your setup code here, to run once:
        
    Serial.begin(9600);   // write out the values
    delay(100);

    // Add the two rotaries for the first encoder.
    // Define a rotary to go from -20 to 20 in increments of 2. Initialize it to 10. Allow it to rollover
    encoder1.AddRotaryCounter(ROTARY_ID1, 10, -20, 20, 2, true );
    
    // Define a rotary to go from 50 to 100 in increments of 10. Initialize it to 70. no rollover    
    encoder1.AddRotaryCounter(ROTARY_ID2, 70, 50, 100, 10, false );

    // Make ROTARY_ID2 active (responds to encoder shaft movements)
    encoder1.SetRotary(ROTARY_ID2);
    Serial.println("Encoder 1 ROTARY_ID2 Active");
    
    encoder1.SetChanged(ROTARY_ID2); // This way we can force an update the first time through

#ifdef TWO_ENCODER_TEST
    //--------------------------------------
    // Now for the second encoder
    // Define a rotary to go from -1000 to -500 in increments of 50. Initialize it to -750. Allow it to rollover
    encoder2.AddRotaryCounter(ROTARY_ID1, -750, -1000, -500, 50, true );
    
    // Define a rotary to go from 0 to 10 in increments of 1. Initialize it to 0. no rollover    
    encoder2.AddRotaryCounter(ROTARY_ID2, 0, 0, 10, 1, false );

    // Define a rotary to go from 200 to 300 in increments of 10. Initialize it to 200. Allow it to rollover
    encoder2.AddRotaryCounter(ROTARY_ID3, 200, 200, 300, 10, true );

    // Make ROTARY_ID2 active (responds to encoder shaft movements)
    encoder2.SetRotary(ROTARY_ID2);
    Serial.println("Encoder 2 ROTARY_ID2 Active");
    
    encoder2.SetChanged(ROTARY_ID2); // This way we can force an update the first time through
#endif
}

void loop() {
    // put your main code here, to run repeatedly:
  
    // On each encoder switch press, toggle between the two rotaries and print their values.
    // Use the IsActive call
  
    if ( encoder1.SwitchPressed() ) {
        if ( encoder1.IsActive(ROTARY_ID1) ) {
            encoder1.SetRotary(ROTARY_ID2);
            Serial.println("Encoder 1 ROTARY_ID2 Active");
        }
        else {
            encoder1.SetRotary(ROTARY_ID1);
            Serial.println("Encoder 1 ROTARY_ID1 Active");
        }
        encoder1.SetChanged(ROTARY_ID2);   // force an update, doesnt matter which one   
    }

#ifdef TWO_ENCODER_TEST
    if ( encoder2.SwitchPressed() ) {
        if ( encoder2.IsActive(ROTARY_ID1) ) {
            encoder2.SetRotary(ROTARY_ID2);
            Serial.println("Encoder 2 ROTARY_ID2 Active");
        }
        else if ( encoder2.IsActive(ROTARY_ID2) ) {
            encoder2.SetRotary(ROTARY_ID3);
            Serial.println("Encoder 2 ROTARY_ID3 Active");
        }
        else {
            encoder2.SetRotary(ROTARY_ID1);
            Serial.println("Encoder 2 ROTARY_ID1 Active");            
        }
        encoder2.SetChanged(ROTARY_ID2);   // force an update, doesnt matter which one   
    }
#endif

#ifdef TWO_ENCODER_TEST
    if ( encoder1.HasRotaryValueChanged(ROTARY_ID1) || encoder1.HasRotaryValueChanged(ROTARY_ID2) ||
         encoder2.HasRotaryValueChanged(ROTARY_ID1) || encoder2.HasRotaryValueChanged(ROTARY_ID2) ||
         encoder2.HasRotaryValueChanged(ROTARY_ID3) ) {
        Serial.print("Encoder 1 ROTARY_ID1: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID1));
        Serial.print("Encoder 1 ROTARY_ID2: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID2));  
        Serial.print("Encoder 2 ROTARY_ID1: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID1));
        Serial.print("Encoder 2 ROTARY_ID2: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID2));  
        Serial.print("Encoder 2 ROTARY_ID3: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID3));
        Serial.println("---------------------------------------");
    }
#else
    if ( encoder1.HasRotaryValueChanged(ROTARY_ID1) || encoder1.HasRotaryValueChanged(ROTARY_ID2) ) {
        Serial.print("Encoder 1 ROTARY_ID1: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID1));
        Serial.print("Encoder 1 ROTARY_ID2: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID2));  
        Serial.println("---------------------------------------");
    }
#endif
}
