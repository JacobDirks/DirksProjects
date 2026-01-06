---
title: Mechanical Clock
tags:
- tag1
- tag2
---

## Overview

<!-- Insert information about the goals of the project and what the project is. -->
Create a several segment clock to learn communication protocols, print statement debugging, and wire management in a physical project.

## Visuals

<!-- Insert Pictures of the project -->
Are not yet ready as the project is in the barest of implementation stages.

## Contribution

<!-- Talk about what parts I did for the project -->
I briefly saw an image of a similar project and wanted to attempt to create everything from scratch. This has thus far been limited by my lack of understanding of communication protocols and rather than risk breaking electrical parts, I enrolled in classes that would teach those communication protocols. Otherwise I have designed the initial layout of the physical components and the code base to match.

## Technical Details

<!-- Insert any technical information that would be desired -->
The current parts for the template are as follows:

* Clock segment or a servo mount
  * ![Clock segment image](images/clockSegmentDrawing.png)
* Clock Face
  * ![Clock face image](images/clockFaceDrawing.png)
* Corner link
  * ![Clock corner link](images/clockCornerDrawing.png)
* Center links
  * ![Clock link image](images/clockLinkDrawing.png)

As for files to print these parts look no further than this [zip folder](ClockProject.zip)

| Item Name | Quantity | Price |
| :------------------ | :------------- | :------ |
| [Arduino Uno](https://www.amazon.com/s?k=arduino+uno&crid=3HS7LSJUDIEZV&sprefix=arduino+un%2Caps%2C242&ref=nb_sb_noss_2) | 1 | $27.60 |
| [SG90 Micro Servo Motor](https://www.amazon.com/10Pcs-Servos-Helicopter-Airplane-Controls/dp/B07MLR1498/ref=sr_1_2_sspa?crid=2P7G8ZSMZV0LR&dib=eyJ2IjoiMSJ9.DO_8huDXG-WCdEl_xxmMGHDTUq8Gy0sMBJu8P0uFAgVhzhgI-8Auign9UCI5rCaoxSxoYndJU5MYyev8C0a6ieWYdkzcgTzYfCfg0Cgzw-5ms2qD_ftnp_WAp6iGd6PH9KNRJEF3-4Nl5oe_stC43c_bBayX0liXO2K9QQAUbg2Dmh9al7SuJ6ZSTZh8fxflVRFM6yjkgBWzHdpXVPr-LDv9YqT8s8fDWtMQqxyiWLtozejDWtPCj6HElPdYyuiXUbApW2n6FGIv7D1uqR5DuTkAs2chwLgGHoaiO4OaNSA.u9U_zUnuJQdYyKSZm65shdvbQavOO6G2fdaWJDlAY2I&dib_tag=se&keywords=SG90%2BMicro%2BServo&qid=1767662914&s=toys-and-games&sprefix=sg90%2Bmicro%2Bservo%2Ctoys-and-games%2C180&sr=1-2-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&th=1) | 22 | $41.20 |
| [PCA9685 16 Channel PWM Servo Motor Driver Board](https://www.amazon.com/dp/B0DFTLMJCX?ref_=ppx_hzsearch_conn_dt_b_fed_asin_title_2) | 2 | $9.99 |
| 3D filament | Unknown | Unknown |

Code Base:

```Arduino { style="--md-codeblock-height: 100px;" }

#include <Wire.h>
#include <Keypad.h>
#include <Adafruit_PWMServoDriver.h>
/*
Made by Jacob Dirks
Dedicated hours spent: 7
Half-focused hours: 22 - I like watching movies and playing games while on summer break
Built for: Personal Project
*/

/*
TODO List: 
Figure out if we're adding light sources. Are those on a switch? - feature for an updated version
Check power draw
Finally make edits for a finalized design per digit, Clean up wiring in that design, clean up code, and get mounting

*/

/*

 DIAGRAM
   -         20             13          6
-    22   18    19       11    12     4   5
   -          17            10          3
-    21    15    16       8     9     1   2
   -          14             7          0
hourTen   hourOne        minuteTen   minuteOne


*/

//section for button sensor it goes from pins 9 to pin 2-3 depending on if you want to connect the last row or not. Since the code doesn't use A-D my model doesn't have it connected
const byte ROWS = 4;
const byte COLS = 4; // if there are 3 columns update this
char hexaKeys[ROWS][COLS] = { // ABCD are later ignored so we could use a 3x4 instead of a 4x4
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; // for 3x4 remove ", 2"

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver driverTwo = Adafruit_PWMServoDriver(0x41);


#define NUM_SERVOS 23
#define SERVO_MIN 100               // SERVO_MIN and SERVO_MAX are based on pulse widths for positioning, other unit information is not generally provided. MIN = 100, MAX = 600 for my servos and values provided by spec sheet.
#define SERVO_MAX 600               // signial to put them on the otherside but is currently unused
#define SERVO_MID 350               // THIS IS DONE BY Max-Min MAY want to update it
#define SERVO_DELAY 1000 // delay in milliseconds

const int SERVO_DRIVER_CAP = 16;
int hourTen;
int hourOne;
int minuteOne;
int minuteTen;
unsigned long prevMilli; // useful for setting up clock while other items are still processing or delays are running
unsigned long currMilli;
unsigned long duration;
unsigned long minuteTime = 60000;  // 60000ms in a minute used for consistent time changes


bool minute = false;


void setup(){
  Serial.begin(9600);
Serial.println("started");
  prevMilli = millis();
  driver.begin();
  driver.setPWMFreq(50);
  delay(50);
Serial.println("Driver initialized");
  // Initialzie all servos to their min position durving setup
  for (int i = 0; i < NUM_SERVOS; i++){

    if (i > SERVO_DRIVER_CAP){
    
    driver.setPWM(i, 0, SERVO_MIN);
    
    } else {
    
      driverTwo.setPWM(i, 0, SERVO_MIN);
    
    }
  
  }
  
  hourTen = 0; // I desire to initalize to 1:00
  hourOne = 1;
  minuteOne = 0;
  minuteTen = 0;

  // initialize the first position to 1:00
  // as we are currently only working with 1 digit we will do the first 0 first.
Serial.println("Starting clock");
 setClock(100);
 delay(10);

}

void loop(){

  currMilli = millis();
  duration = currMilli-prevMilli; // systematically identify how long things have been running for TESTING
  Serial.println(duration);
  if (duration > minuteTime){ // so that's every minute... what about when it flips on day 49.7?                                                     //REAL TIME ERROR  we crash at 60000 ms ... WHY?
  
    // Ok research says that since they are unsigned longs this one line should TECHNICALLY handle the overflow
Serial.println("We made it into the loop and you didn't mess up the math");
  
    changeTime();
    
    }

    char customKey = customKeypad.getKey();

  if (customKey!= NO_KEY){

    inputKeypad();

  }

  delay(100);

}

void inputKeypad(){

  String futureTime = "";
  char lastKeyPress = NO_KEY;
  int desiredTime = 0;

  while ((lastKeyPress != '*') || (lastKeyPress != '#')){ // while character inputed is not # or *  we can continue getting values;
  
    if (lastKeyPress != NO_KEY){// check if its a letter, if not then add it to future time
    
      switch (lastKeyPress){
    
        case 'A':
        case 'B':
        case 'C':
        case 'D': // 'A' 'B' 'C' and 'D' all end up doing the same thing resetting it NO_KEY and searching for a new input
          lastKeyPress = NO_KEY;
          break;
    
        case '*':
        case '#': // with * and # in case they somehow aren't picked up we want the system to recongnize it so we'll give a second chance
          break;
    
        default: // default is for 0-9 and will need input from hours first in tens then ones, then go minutes in tens then ones
          futureTime += lastKeyPress;
          lastKeyPress = NO_KEY;

      /* //alternative case's for legibility

        case 'A': // ignore current input and get new

          lastKeyPress = NO_KEY
          break;
    
        case 'B': // ignore current input and get new

          lastKeyPress = NO_KEY
          break;
    
        case 'C': // still not a valid input so we ignore it

          lastKeyPress = NO_KEY
          break;
    
        case 'D': // also not valid

          lastKeyPress = NO_KEY
          break;
    
        case '*': // with * and # in case they somehow aren't picked up we want the system to recongnize it so we'll give a second chance

          break;
        
        case '#': // should also be redundant

          break;

        default:  // default is for 0-9 and will need input from hours first in tens then ones, then go minutes in tens then ones

          futureTime+= lastKeyPress;
          lastKeyPress = NO_KEY;
      
      */
      }
    }
      

    if (futureTime.length() == 4){ // only 4 digits are allowed in the clock
    
      break; // break out of the while loop chains and go to end commands which relate to sending the number to the clock

    }

    while (lastKeyPress == NO_KEY){
    
      delay(10);
      lastKeyPress = customKeypad.getKey();

    }

  }

  desiredTime = futureTime.toInt(); // convert to an int
  Serial.println(futureTime);                                                                                            // FOR TESTING
  setClock(desiredTime);

}


void setClock(int num){ // When in combination with switch to set time.

  //first lower all planes before we start worrying about the 3-4 digit number getting passed
  closeSevenSegmentDigit(0);
  closeSevenSegmentDigit(7);
  closeSevenSegmentDigit(14);
  closeTwoSegmentDigit(21);

Serial.println("closed segments");

  int digit = num % 10; //isolate last digit- forced to be 0-9
  num = num / 10; // prepare num for next section
  setMinuteOne(digit); // send last digit to lowest place - repeat
  digit = num % 10;
  num = num / 10;
  setMinuteTen(digit);
  digit = num % 10;
  num = num / 10;
  setHourOne(digit);
  setHourTen(num); // num should only be 1 place left and that should be a 1 or a 0. 
  prevMilli = currMilli;

}

void setHourTen(int desired){ // I uh don't know anymore why its a switch case other than unity between methods

  switch(desired){

    case 1:

      moveNumUp(21);
      moveNumUp(22);
      hourTen = desired;

  }

}

void setHourOne(int desired){// raises the individual planes one by one to represent each number for the ones place in the hour location
  
    switch(desired){

    case 0:

      moveNumUp(14);
      moveNumUp(15);
      moveNumUp(16);
      moveNumUp(18);
      moveNumUp(19);
      moveNumUp(20);
      break;
    
    case 1:

      moveNumUp(16);
      moveNumUp(19);
      break;

    case 2:

      moveNumUp(14);
      moveNumUp(15);
      moveNumUp(17);
      moveNumUp(19);
      moveNumUp(20);
      break;

    case 3:

      moveNumUp(14);
      moveNumUp(16);
      moveNumUp(17);
      moveNumUp(19);
      moveNumUp(20);
      break;

    case 4:

      moveNumUp(16);
      moveNumUp(17);
      moveNumUp(18);
      moveNumUp(19);
      break;

    case 5:

      moveNumUp(14);
      moveNumUp(16);
      moveNumUp(17);
      moveNumUp(18);
      moveNumUp(20);
      break;

    case 6:

      moveNumUp(14);
      moveNumUp(15);
      moveNumUp(16);
      moveNumUp(17);
      moveNumUp(18);
      moveNumUp(20);
      break;

    case 7:

      moveNumUp(16);
      moveNumUp(19);
      moveNumUp(20);
      break;
    
    case 8:

      moveNumUp(14);
      moveNumUp(15);
      moveNumUp(16);
      moveNumUp(17);
      moveNumUp(18);
      moveNumUp(19);
      moveNumUp(20);
      break;

    case 9:

      moveNumUp(14);
      moveNumUp(16);
      moveNumUp(17);
      moveNumUp(18);
      moveNumUp(19);
      moveNumUp(20);
      break;

  }

  hourOne = desired;

}

void setMinuteTen(int desired){// raises the individual planes one by one to represent each number for the tens place in the minute location

    switch(desired){

    case 0:

      moveNumUp(7);
      moveNumUp(8);
      moveNumUp(9);
      moveNumUp(11);
      moveNumUp(12);
      moveNumUp(13);
      break;
    
    case 1:

      moveNumUp(9);
      moveNumUp(12);
      break;

    case 2:

      moveNumUp(7);
      moveNumUp(8);
      moveNumUp(10);
      moveNumUp(12);
      moveNumUp(13);
      break;

    case 3:

      moveNumUp(7);
      moveNumUp(9);
      moveNumUp(10);
      moveNumUp(12);
      moveNumUp(13);
      break;

    case 4:

      moveNumUp(9);
      moveNumUp(10);
      moveNumUp(11);
      moveNumUp(12);
      break;

    case 5:

      moveNumUp(7);
      moveNumUp(9);
      moveNumUp(10);
      moveNumUp(11);
      moveNumUp(13);
      break;

    case 6:

      moveNumUp(7);
      moveNumUp(8);
      moveNumUp(9);
      moveNumUp(10);
      moveNumUp(11);
      moveNumUp(13);
      break;

    case 7:

      moveNumUp(9);
      moveNumUp(12);
      moveNumUp(13);
      break;
    
    case 8:

      moveNumUp(7);
      moveNumUp(8);
      moveNumUp(9);
      moveNumUp(10);
      moveNumUp(11);
      moveNumUp(12);
      moveNumUp(13);
      break;

    case 9:

      moveNumUp(7);
      moveNumUp(9);
      moveNumUp(10);
      moveNumUp(11);
      moveNumUp(12);
      moveNumUp(13);
      break;
  }
  
  minuteTen = desired; 

}

void setMinuteOne(int desired){// raises the individual planes one by one to represent each number for the ones place in the minute location

  switch(desired){

    case 0:

      moveNumUp(0);
      moveNumUp(1);
      moveNumUp(2);
      moveNumUp(4);
      moveNumUp(5);
      moveNumUp(6);
      break;
    
    case 1:

      moveNumUp(2);
      moveNumUp(5);
      break;

    case 2:

      moveNumUp(0);
      moveNumUp(1);
      moveNumUp(3);
      moveNumUp(5);
      moveNumUp(6);
      break;

    case 3:

      moveNumUp(0);
      moveNumUp(2);
      moveNumUp(3);
      moveNumUp(5);
      moveNumUp(6);
      break;

    case 4:

      moveNumUp(2);
      moveNumUp(3);
      moveNumUp(4);
      moveNumUp(5);
      break;

    case 5:

      moveNumUp(0);
      moveNumUp(2);
      moveNumUp(3);
      moveNumUp(4);
      moveNumUp(6);
      break;

    case 6:

      moveNumUp(0);
      moveNumUp(1);
      moveNumUp(2);
      moveNumUp(3);
      moveNumUp(4);
      moveNumUp(6);
      break;

    case 7:

      moveNumUp(2);
      moveNumUp(5);
      moveNumUp(6);
      break;
    
    case 8:

      moveNumUp(0);
      moveNumUp(1);
      moveNumUp(2);
      moveNumUp(3);
      moveNumUp(4);
      moveNumUp(5);
      moveNumUp(6);
      break;

    case 9:

      moveNumUp(0);
      moveNumUp(2);
      moveNumUp(3);
      moveNumUp(4);
      moveNumUp(5);
      moveNumUp(6);
      break;
  }
  
  minuteOne = desired;

}

void closeSevenSegmentDigit(int startNum){ // Puts all of the segments in that area down. Only works on minutes and the ones position for the hours

  for (int i = startNum; i<(startNum+7); i++){

    moveNumDown(i);

  }

  delay(10);

}

void closeTwoSegmentDigit(int startNum){ // lowers both segments of the hour section and potientially any other's that you want to lower 2 sections on

  for (int i = startNum; i<(startNum+2); i++){

    moveNumDown(i);

  }

  delay(10);

}

void changeTime(){ // assumptions = increase hour will tell if we need to increase the tens place as well and if we need to reset it to 1. Increase Minute will handle both 10s and 1s

  if (minuteOne == 9){
    
    incMinuteOne(0); // set's 1's place to 0

    if (minuteTen == 5){ // logic for incrementing 10's place

      increaseHour();
      incMinuteTen(0);

    } else {

    incMinuteTen(minuteTen+1);

    }

  } else {
    
    incMinuteOne(minuteOne + 1);

  }

  prevMilli = currMilli - (currMilli % minuteTime); // this updates the minute changes without losing seconds
  Serial.println(prevMilli);
  Serial.print("Minute One: ");
  Serial.println(minuteOne);
}

void increaseHour(){
  
  if (hourTen==1){ // edge case of 12:59 increment

    if (hourOne == 2){

      incHourTen(0);
      incHourOne(12); // specialized edge case

    }

  } else if (hourOne ==9){ //go to 10:00 

    incHourTen(1);
    incHourOne(0);

  }  else {

    incHourOne(hourOne + 1); // standard increment

  }

}


void incHourTen(int desired){ 

  switch(desired){

    case 1:

      //switch from 0 to 1
      moveNumUp(21);
      moveNumUp(22);
      break;
    
    case 0:

      //switch from 1 to 0
      moveNumDown(21);
      moveNumDown(22);
      break;

  }

  hourTen = desired;

}

void incHourOne(int desired){ // this one is needed to go from 12 back to 01

  switch(desired){

    case 0: // 9 to 0

      moveNumDown(17);
      moveNumUp(15);
      incHourTen(1);
      break;
    
    case 1: // 0 to 1

      moveNumDown(14);
      moveNumDown(15);
      moveNumDown(17);
      moveNumDown(18);
      moveNumDown(20);
      break;

    case 2: // 1 to 2

      moveNumDown(16);
      moveNumUp(14);
      moveNumUp(15);
      moveNumUp(17);
      moveNumUp(20);
      break;

    case 3: // 2 to 3

      moveNumDown(15);
      moveNumUp(16);
      break;

    case 4: // 3 to 4

      moveNumDown(14);
      moveNumDown(20);
      moveNumUp(18);
      break;
      
    case 5:  // 4 to 5

      moveNumDown(19);
      moveNumUp(14);
      moveNumUp(17);
      moveNumUp(18);
      moveNumUp(20);
      break;

    case 6:  // 5 to 6

      moveNumUp(15);
      break;

    case 7:  // 6 to 7

      moveNumDown(14);
      moveNumDown(15);
      moveNumDown(17);
      moveNumDown(18);
      moveNumUp(19);
      break;

    case 8: // 7 to 8

      moveNumUp(14);
      moveNumUp(15);
      moveNumUp(17);
      moveNumUp(18);
      break;

    case 9: // 8 to 9

      moveNumDown(15);
      break;

    case 12: // 2 to 1 for 12 to 01 edge case

      moveNumDown(0);
      moveNumDown(1);
      moveNumDown(3);
      moveNumDown(6);
      moveNumUp(2);
      break;

  }

  hourOne = desired;
  
}

void incMinuteTen(int desired){
  
  switch(desired){

    case 1: // 0 to 1

      moveNumDown(7);
      moveNumDown(8);
      moveNumDown(10);
      moveNumDown(11);
      moveNumDown(13);
      break;
    
    case 2: // 1 to 2

      moveNumDown(9);
      moveNumUp(7);
      moveNumUp(8);
      moveNumUp(10);
      moveNumUp(13);
      break;

    case 3: // 2 to 3

      moveNumDown(8);
      moveNumUp(9);
      break;

    case 4: // 3 to 4

      moveNumDown(7);
      moveNumDown(13);
      moveNumUp(11);
      break;

    case 5: // 4 to 5

      moveNumDown(12);
      moveNumUp(7);
      moveNumUp(13);
      break;

    case 0: // 5 to 0

      moveNumDown(10);
      moveNumUp(8);
      moveNumUp(12);
      break;

  }

  minuteTen = desired;
  
}

void incMinuteOne(int desired){
  
  switch(desired){                                                                                            //WORRY about Case 1?????

    case 1:

      // change 0 to 1
      moveNumDown(0);
      moveNumDown(1);
      moveNumDown(3);
      moveNumDown(4);
      moveNumDown(6);
      break;

    case 2:

      //change 1 to 2 
      moveNumDown(2);
      moveNumUp(1);
      moveNumUp(6);
      moveNumUp(3);
      moveNumUp(0);
      break;
    
    case 3:

      // change 2 to 3
      moveNumDown(1);
      moveNumUp(2);
      break;

    case 4:

      //change 3 to 4
      moveNumDown(6);
      moveNumDown(0);
      moveNumUp(4);
      break;

    case 5:

      //change 4 to 5
      moveNumDown(5);
      moveNumUp(6);
      moveNumUp(0);
      break;

    case 6:

      // change 5 to 6
      moveNumUp(1);
      break;

    case 7:

      // change 6 to 7
      moveNumDown(0);
      moveNumDown(1);
      moveNumDown(3);
      moveNumDown(4);
      moveNumUp(2);
      moveNumUp(5);
      break;

    case 8:

      // change 7 to 8
      moveNumUp(4);
      moveNumUp(3);
      moveNumUp(1);
      moveNumUp(0);
      break;

    case 9:

      // change 8 to 9
      moveNumDown(1);
      moveNumDown(0);
      break;

    case 0:
      
      //change 9 to 0
      moveNumDown(3);
      moveNumUp(1);
      moveNumUp(0);
      setMinuteTen(minuteTen++);
      break;
    
    }

    minuteOne = desired; 

}



// This section is not to be manipulated past testing. It functions as intended and will safely raise and lower bars

// Num means its connected to the servo controller
// Pin means its directly connected to the servo controller. 
void moveNumUp(int servoNum){

  if (servoNum > SERVO_DRIVER_CAP){

    for (int pos = SERVO_MIN; pos<=SERVO_MID; pos++){

        driver.setPWM(servoNum, 0, pos);
        delay(5);

    }

    delay(10);
    driver.setPWM(servoNum,0, SERVO_MID);

  } else {

    for (int pos = SERVO_MIN; pos<=SERVO_MID; pos++){

      driverTwo.setPWM(servoNum, 0, pos);
      delay(5);

    }

    delay(10);
    driverTwo.setPWM(servoNum,0, SERVO_MID);

  }

}

void moveNumDown(int servoNum){

  if (servoNum > SERVO_DRIVER_CAP){

    for (int pos = SERVO_MID; pos>=SERVO_MIN; pos--){

      driver.setPWM(servoNum, 0, pos);
      delay(5);

    }

    delay(10);
    driver.setPWM(servoNum,0, SERVO_MIN);

  } else {

    for (int pos = SERVO_MID; pos>=SERVO_MIN; pos--){

      driverTwo.setPWM(servoNum, 0, pos);
      delay(5);

    }

    delay(10);
    driverTwo.setPWM(servoNum,0, SERVO_MIN);

  }

}

```

## Results

<!-- Did we meet project reqs, did we achieve initially goals? -->
This project is currently not complete so we have not achieved all of the goals of the project.
