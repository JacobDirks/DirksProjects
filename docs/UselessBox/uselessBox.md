---
title: Useless Box
tags:
- tag1
- tag2
---

## Overview

<!-- Insert information about the goals of the project and what the project is. -->
Create a project to learn the basics of a pull up switch and motor using Arduino while ensuring the design is simple and replication is easy. If the goal is to view the project components please look at the Bill of Materials under the technical section and download the [3D printing files](project_files/UselessBox.zip) and the [code base.](project_files/UselessBoxRefactoring.ino)

<!-- ## Visuals -->

<!-- Insert Pictures of the project -->

## Contribution

<!-- Talk about what parts I did for the project -->
This was an individual project which allowed me to work through all aspects of the project from initial conception to testing to final completion.

## Technical Details

<!-- Insert any technical information that would be desired -->
![useless Box Basic drawing](images/uselessBoxDrawing.png)

| Item Name         | Quantity     | Price |
| [Arduino Uno](https://www.amazon.com/s?k=arduino+uno&crid=3HS7LSJUDIEZV&sprefix=arduino+un%2Caps%2C242&ref=nb_sb_noss_2) | 1 | $27.60 |
| 3D print filament | 187.74 grams | ~$3.20 |
| Servo Futaba S3003 | 1            | $6.25 |
| [3 position switch](https://www.amazon.com/EGSCST-Toggle-Switch-Miniature-Position/dp/B0FH2B67R2/ref=sxin_17_pa_sp_search_thematic_sspa?content-id=amzn1.sym.439af9a8-30da-494e-93f0-bd598e1aa4ee%3Aamzn1.sym.439af9a8-30da-494e-93f0-bd598e1aa4ee&crid=G4LYLY6REFL5&cv_ct_cx=3%2Bposition%2Bswitch&keywords=3%2Bposition%2Bswitch&pd_rd_i=B0FH2B67R2&pd_rd_r=4d1e4dce-4b64-4ad0-810b-12d115bc404d&pd_rd_w=eb0VZ&pd_rd_wg=1mi6l&pf_rd_p=439af9a8-30da-494e-93f0-bd598e1aa4ee&pf_rd_r=V8B43HB9JP4GTGR0D6HR&qid=1767649990&sbo=RZvfv%2F%2FHxDF%2BO5021pAnSA%3D%3D&sprefix=3%2Bposition%2Bswitc%2Caps%2C199&sr=1-2-6e60e730-e094-43e9-99e8-1a4854cd27ff-spons&aref=g7me5EJKsc&sp_csd=d2lkZ2V0TmFtZT1zcF9zZWFyY2hfdGhlbWF0aWM&th=1) | 1        | $0.67 |
| [Copper wires](https://www.amazon.com/gp/aw/d/B0C2TZYFDC/?_encoding=UTF8&pd_rd_plhdr=t&aaxitk=d58739e70afbfc07701e855c83c6caf8&hsa_cr_id=0&qid=1767650116&sr=1-2-f02f01d6-adaf-4bef-9a7c-29308eff9043&ref_=sbx__sbtcd2_asin_1_img&pd_rd_w=GXV7g&content-id=amzn1.sym.e2c9099f-6964-4dbf-9ce9-8bc2c1a8ec1a%3Aamzn1.sym.e2c9099f-6964-4dbf-9ce9-8bc2c1a8ec1a&pf_rd_p=e2c9099f-6964-4dbf-9ce9-8bc2c1a8ec1a&pf_rd_r=4QKAGXN9ZVTXBN0G20F8&pd_rd_wg=fHweo&pd_rd_r=ce2dcbd2-44ce-4053-a045-71e9fd8594a2&th=1) | 1      | $3.67 |

Arduino Code Base:

```

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
          
          case 3:/* a slow and then come back only to go out and hold it down again */

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

```

## Results

<!-- Did we meet project reqs, did we achieve initially goals? -->
I was able to successfully build, test, and improve the useless box. Although there were several complications along the way regarding CAD software, signal reading, and physical manipulations.

| System/Software | Lessons Learned |
| :--------------- | :-------------------------------------------- |
| Solidworks | Planes on an object, extrusion about an axis, print in place features, and tolerance printing |
| Arduino | Importance of simplifying complex sections in code, highlighting reusable parts, and the blessings of commented code |

Future improvements:
Reduce the footprint of the box, optimize port usage versus numbers, and add more reactions for the switch.
