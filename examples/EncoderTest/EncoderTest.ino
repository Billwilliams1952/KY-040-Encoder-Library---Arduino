/*
 * EncoderTest.ino
 * 2016 WLWilliams
 * 
 * This sketch demonstrates the use of the KY-040 encoder library.  The code supports testing
 * up to four encoders at once.  How m,any you can test depends on your Arduino type.
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

/*
 * Define how many encoders you want to test: 1 to 4
 */
#define NUMBER_OF_ENCODERS   1      // This code supports 1 to 4
#define BOARD_LED           13      // I'm alive blinker
/*
 * Error output messages
 */
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  const char msg[] = "Only pins 2 and 3 are interrupt pins";
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  const char msg[] = "Only pins 2, 3, 18, 19, 20, and 21 are interrupt pins";
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
  const char msg[] = "Only pins 0, 1, 2, 3, and 7 are interrupt pins";
#else
  // TODO: Add more checks for other types..... Adafruit trinkets, etc.
  const char msg[] = "Unknown processor type. Check your documentation for interrupt pins";
#endif
/* 
 * Define some rotary IDs. IDs can range from 0 to 254. 255 is reserved for the active one
 * ( CURRENT_ID )
 */
#define ROTARY_ID1           1 
#define ROTARY_ID2          10 
#define ROTARY_ID3         100
/*
 * Create encoders. 
 * Define how the encoder is connected to the Arduino. ENCODER_CLK is the interrupt pin
 */
// First Encoder
#define ENCODER_CLK1         2      // This pin must have a minimum 0.47 uF capacitor
#define ENCODER_DT1         A0      // data pin
#define ENCODER_SW1         A1      // switch pin (active LOW)
#define MAX_ROTARIES1        2      // this example defines two rotaries for this encoder
ky040 encoder1(ENCODER_CLK1, ENCODER_DT1, ENCODER_SW1, MAX_ROTARIES1 );

#if NUMBER_OF_ENCODERS >= 2
  #define ENCODER_CLK2         3      // This pin must have a minimum 0.47 uF capacitor    
  #define ENCODER_DT2         A2      // data pin
  #define ENCODER_SW2         A3      // switch pin (active LOW)
  #define MAX_ROTARIES2        3      // this example defines three rotaries for this encoder
  ky040 encoder2(ENCODER_CLK2, ENCODER_DT2, ENCODER_SW2, MAX_ROTARIES2 );
#endif

#if NUMBER_OF_ENCODERS >= 3
  // Assume an Arduino Micro is installed.... change as necessary
  #define ENCODER_CLK3         0      // This pin must have a minimum 0.47 uF capacitor    
  #define ENCODER_DT3         A4      // data pin
  #define ENCODER_SW3         A5      // switch pin (active LOW)
  #define MAX_ROTARIES3        1      // this example defines one rotary for this encoder
  ky040 encoder3(ENCODER_CLK3, ENCODER_DT3, ENCODER_SW3, MAX_ROTARIES3 );
#endif

#if NUMBER_OF_ENCODERS == 4
  // Assume an Arduino Micro is installed.... change as necessary
  #define ENCODER_CLK4         7      // This pin must have a minimum 0.47 uF capacitor    
  #define ENCODER_DT4         12      // data pin
  #define ENCODER_SW4         11      // switch pin (active LOW)
  #define MAX_ROTARIES4        1      // this example defines one rotary for this encoder
  ky040 encoder4(ENCODER_CLK4, ENCODER_DT4, ENCODER_SW4, MAX_ROTARIES4 );
#endif

void Error ( int encoderNum ) {
    Serial.print("ERROR creating encoder "); Serial.print(encoderNum); Serial.println("!");
    Serial.println(msg);
    Serial.println("Possible malloc error, the rotary ID is incorrect, or too many rotaries");
    while(1);   // Halt and catch fire
}

void setup() {
    pinMode(BOARD_LED,OUTPUT);
    digitalWrite(BOARD_LED,LOW);
        
    Serial.begin(9600) ;   // write out the values. Make sure the Terminal is also set to 9600
    while ( ! Serial ) ;


#if NUMBER_OF_ENCODERS < 1 || NUMBER_OF_ENCODERS > 4
    Serial.println("Wrong number of encoders have been defined! ( expecting 1 to 4 )");
    while(1);
#endif

#if NUMBER_OF_ENCODERS >= 1
    // NOTE: The AddRotaryCounter function is declared a void type if SMALLEST_CODESIZE if defined in ky040.h
    
    // Add the two rotaries for the first encoder.
    // Define a rotary to go from -20 to 20 in increments of 2. Initialize it to 10. Allow it to rollover
    if ( ! encoder1.AddRotaryCounter(ROTARY_ID1, 10, -20, 20, 2, true ) ) {
      Error(1);
    }

    // Define a rotary to go from 50 to 100 in increments of 10. Initialize it to 70. no rollover    
    if ( ! encoder1.AddRotaryCounter(ROTARY_ID2, 70, 50, 100, 10, false ) ) {
      Error(1);
    }

    // Make ROTARY_ID2 active (responds to encoder shaft movements)
    encoder1.SetRotary(ROTARY_ID2);
#endif

#if NUMBER_OF_ENCODERS >= 2
    // Define a rotary to go from -1000 to -500 in increments of 50. Initialize it to -750. Allow it to rollover
    if ( ! encoder2.AddRotaryCounter(ROTARY_ID1, -750, -1000, -500, 50, true ) ) {
      Error(2);
    }
    
    // Define a rotary to go from 0 to 10 in increments of 1. Initialize it to 0. no rollover    
    if ( ! encoder2.AddRotaryCounter(ROTARY_ID2, 0, 0, 10, 1, false ) ) {
      Error(2);
    }
    
    // Define a rotary to go from 200 to 300 in increments of 10. Initialize it to 200. Allow it to rollover
    if ( ! encoder2.AddRotaryCounter(ROTARY_ID3, 200, 200, 300, 10, true ) ) {
      Error(2);
    }
    
    // Make ROTARY_ID2 active (responds to encoder shaft movements)
    encoder2.SetRotary(ROTARY_ID2);
#endif

#if NUMBER_OF_ENCODERS >= 3
    // Now for the third encoder.  Make a generic counter 0 to 20 with rollover
    if ( ! encoder3.AddRotaryCounter(ROTARY_ID1, 20, true ) ) {
      Error(3);   
    }
#endif

#if NUMBER_OF_ENCODERS == 4
    // Now for the fourth encoder.  Make a generic counter 0 to 10 with no rollover
    if ( ! encoder4.AddRotaryCounter(ROTARY_ID1, 10 ) ) {
      Error(4);  
    }
#endif

}

void loop() {
    static bool firstTime = true;   // Force an initial display of rotary values
    
    digitalWrite(BOARD_LED,millis() % 1000 > 500);  // Blink I'm alive   

 #if NUMBER_OF_ENCODERS >= 1
    // On each encoder switch press, toggle between the two rotaries.
    // Use the IsActive call 
    if ( encoder1.SwitchPressed() or firstTime ) {
        Serial.print("Encoder 1 switch pressed. ");
        if ( encoder1.IsActive(ROTARY_ID1) ) {
            encoder1.SetRotary(ROTARY_ID2);
            Serial.println("ROTARY_ID2 now Active.");
        }
        else {
            encoder1.SetRotary(ROTARY_ID1);
            Serial.println("ROTARY_ID1 now Active.");
        }
        encoder1.SetChanged();   // force an update on active rotary   
    }
#endif

#if NUMBER_OF_ENCODERS >= 2
    // On each encoder switch press, toggle between the three rotaries.
    if ( encoder2.SwitchPressed() ) {
        Serial.print("Encoder 2 switch pressed. ");
        if ( encoder2.IsActive(ROTARY_ID1) ) {
            encoder2.SetRotary(ROTARY_ID2);
            Serial.println("ROTARY_ID2 now Active.");
        }
        else if ( encoder2.IsActive(ROTARY_ID2) ) {
            encoder2.SetRotary(ROTARY_ID3);
            Serial.println("ROTARY_ID3 now Active.");
        }
        else {
            encoder2.SetRotary(ROTARY_ID1);
            Serial.println("ROTARY_ID1 now Active.");            
        }
        encoder2.SetChanged();   // force an update on active rotary   
    }
#endif

#if NUMBER_OF_ENCODERS >= 3
    if ( encoder3.SwitchPressed() ) {
      // There is only one rotary defined in this case.
      Serial.println("Encoder 3 switch pressed.");
    }
#endif

#if NUMBER_OF_ENCODERS == 4
    if ( encoder4.SwitchPressed() ) {
      // There is only one rotary defined in this case.
      Serial.println("Encoder 4 switch pressed.");
    }
#endif

#if NUMBER_OF_ENCODERS == 1
    if ( encoder1.HasRotaryValueChanged() ) {
        Serial.print("Encoder 1 ROTARY_ID1: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID1));
        Serial.print("Encoder 1 ROTARY_ID2: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID2)); 
        
        Serial.println("---------------------------------------"); 
    }
#elif NUMBER_OF_ENCODERS == 2
    if ( encoder1.HasRotaryValueChanged() || encoder2.HasRotaryValueChanged() ) {
        Serial.print("Encoder 1 ROTARY_ID1: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID1));
        Serial.print("Encoder 1 ROTARY_ID2: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID2));  
        Serial.print("Encoder 2 ROTARY_ID1: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID1));
        Serial.print("Encoder 2 ROTARY_ID2: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID2));  
        Serial.print("Encoder 2 ROTARY_ID3: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID3));
        
        Serial.println("---------------------------------------");
    }
#elif NUMBER_OF_ENCODERS == 3
    if ( encoder1.HasRotaryValueChanged() || encoder2.HasRotaryValueChanged() ||
         encoder3.HasRotaryValueChanged() ) {
        Serial.print("Encoder 1 ROTARY_ID1: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID1));
        Serial.print("Encoder 1 ROTARY_ID2: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID2));  
        Serial.print("Encoder 2 ROTARY_ID1: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID1));
        Serial.print("Encoder 2 ROTARY_ID2: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID2));  
        Serial.print("Encoder 2 ROTARY_ID3: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID3));
        // Since there is only one rotary for #3 encoder, we can just ask for current (active) one
        Serial.print("Encoder 3 ROTARY_ID1: "); Serial.println(encoder3.GetRotaryValue());
        
        Serial.println("---------------------------------------");
    }
#elif NUMBER_OF_ENCODERS == 4
    if ( encoder1.HasRotaryValueChanged() || encoder2.HasRotaryValueChanged() ||
         encoder3.HasRotaryValueChanged() || encoder4.HasRotaryValueChanged() ) {
        Serial.print("Encoder 1 ROTARY_ID1: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID1));
        Serial.print("Encoder 1 ROTARY_ID2: "); Serial.println(encoder1.GetRotaryValue(ROTARY_ID2));  
        Serial.print("Encoder 2 ROTARY_ID1: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID1));
        Serial.print("Encoder 2 ROTARY_ID2: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID2));  
        Serial.print("Encoder 2 ROTARY_ID3: "); Serial.println(encoder2.GetRotaryValue(ROTARY_ID3));
        // Since there is only one rotary for #3 and #4 encoders, we can just ask for current (active) ones
        Serial.print("Encoder 3 ROTARY_ID1: "); Serial.println(encoder3.GetRotaryValue());
        Serial.print("Encoder 4 ROTARY_ID1: "); Serial.println(encoder4.GetRotaryValue());
        
        Serial.println("---------------------------------------");
    }
#endif

    firstTime = false;    // Now only update display if a rotary value has changed.
}
