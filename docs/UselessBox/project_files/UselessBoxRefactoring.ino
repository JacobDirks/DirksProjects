#include <Servo.h>
Servo ctrlArm; // create servo object to control a servo

// Hours spent refactoring: .75 
// Version 1.1
// Created by Jacob Dirks

// two of twelve ' board limit' reached'

int pos = 0; // variable to control ctrl arm position
int altCount = 0;
int shortHeight = 150; // What is this
int count = 1; // this is what we'll use for the random case generator. Makes it easier to store number
int caseCount = 5; // easier to update later
int runCount = 0;
int moveBox = 3; // stores number 3 for pin 3
int TState = 0; //variable for storing switch status
int TStatePrior = TState; // stores previous state

void setup() {
  // put your setup code here, to run once:
  ctrlArm.attach(13);
  pinMode(moveBox, INPUT_PULLUP); // sets it as a special kind of input which is how the lever requires it

  ctrlArm.write(pos); // gives it the first desire to go there
  delay(100);
  ctrlArm.write(pos); // allows the time to get there or is this what causes the initial open and close
  delay(50);

}

void loop() {

// section to open and move the box
TState = digitalRead(moveBox); // reads state of switch

if((TState == 1) && (TStatePrior == 0)) { // changes the arm to replace it
    if(pos != 170){
      count = ((int)random(1, 2*caseCount) % caseCount);

      switch(count){
          case 0: // fast bounces cause angry

              for(runCount=0; runCount<=7; runCount += 1){

                for(pos = 20; pos <=45 ; pos+= 5){

              ctrlArm.write(pos);
              delay(10); // waits 10ms for the servo to reach the position

              }
                for(pos=45; pos>=20; pos-=1){
                  ctrlArm.write(pos);
                  delay(15);
                }
              } 

              delay(20);

              closeLidSpeed(15);
              break;
          case 2: // just close the lid
            closeLidSpeed(15);
            delay(20);
            break;
          
          case 3:// a slow and then come back only to go out and hold it down again                                          Case 3 is having errors. Not seeing the goal I want from it at all.... it seems defaulted.

            closeLidSpeed(25);
            delay(30);
            closeLidSpeed(25);
            delay(180);
            break;

          case 4: // slow but up down a few times
            closeLidSpeed(20);

            for(runCount=0; runCount < 2; runCount+= 1){

              for(pos=20; pos<=80; pos+= 2){

                ctrlArm.write(pos);
                delay(15);

              }

              for(pos=80; pos >=20; pos-= 2){

                ctrlArm.write(pos);
                delay(15);

              }

            }

            closeLidSpeed(15);

          default:

           closeLidSpeed(30);
            break;
        }
      }
      
        //here is the common code

      TStatePrior = TState; // stores previous Tstate
      TState = digitalRead(moveBox); // updates TState
    } //end if of tstate = 1 and prior = 0
delay(250);
  if((TState == 0) && (TStatePrior ==1)) {

  if(pos != 20){ // if not at home then return there
  for (pos = 170; pos >= 20; pos -= 10) { // goes from 170 degrees to 20 degrees 
  
    ctrlArm.write(pos); // tell servo to go to position in variable 'pos'
    delay(15); // waits 15ms for the servo to reach the position

  }
  }
  TState = digitalRead(moveBox);
TStatePrior = 0;

} else {
  delay(250);
  TState = digitalRead(moveBox);
}

} 

void closeLidSpeed(int speed){ // a method to simply flip the switch back with a delay choice
  for (pos = 20; pos<= 170; pos+=10){
    ctrlArm.write(pos);
    delay(speed); // wait 15 ms and then move it
  }
  ctrlArm.write(170);
  pos = 170;
}
