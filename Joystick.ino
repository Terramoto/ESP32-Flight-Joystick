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

BleGamepad bleGamepad("Stick do Caralho","João Terramoto",100);

//Axis Pins
const short axisPins[] = {34, 36, 38, 39};
const short pinCount = 4;
const short numberOfPotSamples = 5;     // Number of pot samples to take (to smooth the values)
const short delayBetweenSamples = 2;    // Delay in milliseconds between pot samples
const short delayBetweenHIDReports = 5; // Additional delay in milliseconds between HID reports


void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  
  //Button's Pins
  pinMode(17,INPUT_PULLUP);
  pinMode(18,INPUT_PULLUP);
  pinMode(19,INPUT_PULLUP);
  pinMode(21,INPUT_PULLUP);
  bleGamepad.begin();
}

void loop() {
  if(bleGamepad.isConnected()){
    short buttonPins[]={17,18,19,21};
    short buttonIds[]={1,2,4,8};
    
    for(short a = 0; a<4;a++){
        if(digitalRead(buttonPins[a])==LOW){
          bleGamepad.press(buttonIds[a]);
        }else{
          bleGamepad.release(buttonIds[a]);
        }
    }
    /*int potValues[numberOfPotSamples];  // Array to store pot readings
    int adjustedValue[] = {0,0,0,0};// Variable to store calculated pot reading average
    
    // Populate readings
    for(int a = 0; a < pinCount; a++)
     {
      for(int i = 0 ; i < numberOfPotSamples ; i++)
       {
        potValues[i] = analogRead(axisPins[pinCount]);
        delay(delayBetweenSamples);
      }
      
      int potValue = 0;
      
      // Iteraate through the readings to sum the values
      for (int i = 0 ; i < numberOfPotSamples ; i++)
      {
       potValue += potValues[i]; 
      }
      // Calculate the average
      potValue = potValue / numberOfPotSamples;
      // Map analog reading from 0 ~ 4095 to 127 ~ -127 for use as an axis reading
      adjustedValue[a] = map(potValue, 0, 4095, 127, -127);
    }
    bleGamepad.setAxes(adjustedValue[0], adjustedValue[1], adjustedValue[2], adjustedValue[3], 0, 0, DPAD_CENTERED);*/
    short x =map(analogRead(36), 0, 4095, 127, -127);
    short y =map(analogRead(34), 0, 4095, 127, -127);
    short z =map(analogRead(35), 0, 4095, 127, -127);
    short r =map(analogRead(39), 0, 4095, 127, -127);
    /*Serial.print("X:");
    Serial.println(analogRead(36));
    Serial.print("Y:");
    Serial.println(analogRead(34));
    Serial.print("Z:");
    Serial.println(analogRead(35));
    Serial.print("R:");
    Serial.println(analogRead(39));*/
    
    bleGamepad.setAxes(x, y, z, r, 0, 0, DPAD_CENTERED);
    delay(delayBetweenHIDReports);
  }
}
