/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 * 
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_14* 
 * 
 * Possible DPAD/HAT switch position values are: 
 * DPAD_CENTERED, DPAD_UP, DPAD_UP_RIGHT, DPAD_RIGHT, DPAD_DOWN_RIGHT, 
 * DPAD_DOWN, DPAD_DOWN_LEFT, DPAD_LEFT, DPAD_UP_LEFT
 * 
 * bleGamepad.setAxes takes the following signed char parameters: 
 * (Left Thumb X, Left Thumb Y, Right Thumb X, Right Thumb Y, Left Trigger, Right Trigger, Hat switch position);
 */
 
#include <BleGamepad.h> 
#include "ibus.h"

// How often to send data?
#define UPDATE_INTERVAL 10 // milliseconds

// 1. Analog channels. Data can be read with the Arduino's 10-bit ADC.
// This gives values from 0 to 1023.
// Specify below the analog pin numbers (as for analogRead) you would like to use.
// Every analog input is sent as a single channel.
#define NUM_ANALOG_INPUTS 4 // Axis
byte axisPins[]={34,32,33,35}; // element count MUST be == NUM_ANALOG_INPUTS
#define NUM_DIGITAL_INPUTS 4 // Buttons
byte buttonPins[]={17,18,19,21}; // Button Pins
#define ANALOG_REFERENCE EXTERNAL

#define NUM_DIGITAL_BITMAPPED_INPUTS 0
byte digitalBitmappedPins[] = {8, 9, 10};
#define NUM_CHANNELS ( (NUM_ANALOG_INPUTS) + (NUM_DIGITAL_INPUTS) + (15 + (NUM_DIGITAL_BITMAPPED_INPUTS))/16 )
IBus ibus(NUM_CHANNELS);

BleGamepad bleGamepad("Stick do Caralho","Joao Terramoto",100);

const short numberOfPotSamples = 5;     // Number of pot samples to take (to smooth the values)
const short delayBetweenSamples = 2;    // Delay in milliseconds between pot samples
const short delayBetweenHIDReports = 5; // Additional delay in milliseconds between HID reports

void setup() {
  //analogReference(ANALOG_REFERENCE);
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  
  //Button's Pins
  pinMode(17,INPUT_PULLUP);
  pinMode(18,INPUT_PULLUP);
  pinMode(19,INPUT_PULLUP);
  pinMode(21,INPUT_PULLUP);
  pinMode(34,INPUT);
  pinMode(32,INPUT);
  pinMode(33,INPUT);
  pinMode(35,INPUT);
  bleGamepad.begin();
}

//int axisPins[]={34,32,33,35};
//short buttonPins[]={17,18,19,21};
short buttonIds[]={1,2,4,8};
int adjustedValue[] = {0,0,0,0};

void loop() {
  if(bleGamepad.isConnected()){
    int potValues[numberOfPotSamples];
    for(int a=0;a<4;a++)
    {
      // Populate readings
      for(int i = 0 ; i < numberOfPotSamples ; i++)
      {
        potValues[i] = analogRead(axisPins[a]);
        delay(delayBetweenSamples);
      }
      
      int potValue = 0;   // Variable to store calculated pot reading average
      // Iteraate through the readings to sum the values
      for(int i = 0 ; i < numberOfPotSamples ; i++)
      {
        potValue += potValues[i];
      }
      potValue = potValue / numberOfPotSamples;
      adjustedValue[a] = map(potValue, 0, 4095, 127, -127); 
    }
    
    for(short a = 0; a<4;a++){
        if(digitalRead(buttonPins[a])==LOW){
          bleGamepad.press(buttonIds[a]);
        }else{
          bleGamepad.release(buttonIds[a]);
        }
    }

    bleGamepad.setAxes(adjustedValue[0], adjustedValue[1], adjustedValue[2], adjustedValue[3], 0, 0, DPAD_CENTERED);
    delay(delayBetweenHIDReports);
    //bleGamepad.setBatteryLevel();
  }else{
    int i, bm_ch = 0;
  unsigned long time = millis();

  ibus.begin();

  // read analog pins - one per channel
  for(i=0; i < NUM_ANALOG_INPUTS; i++)
    ibus.write(analogRead(axisPins[i]));

  // read digital pins - one per channel
  for(i=0; i < NUM_DIGITAL_INPUTS; i++)
    ibus.write(digitalRead(buttonPins[i]) == HIGH ? 1023 : 0);

  // read digital bit-mapped pins - 16 pins go in one channel
  for(i=0; i < NUM_DIGITAL_BITMAPPED_INPUTS; i++) {
    int bit = i%16;
    if(digitalRead(digitalBitmappedPins[i]) == HIGH)
      bm_ch |= 1 << bit;

    if(bit == 15 || i == NUM_DIGITAL_BITMAPPED_INPUTS-1) {
      // data for one channel ready
      ibus.write(bm_ch);
      bm_ch = 0;
    }
  }

  ibus.end();

  time = millis() - time; // time elapsed in reading the inputs
  if(time < UPDATE_INTERVAL)
    // sleep till it is time for the next update
    delay(UPDATE_INTERVAL  - time);
  }
}
