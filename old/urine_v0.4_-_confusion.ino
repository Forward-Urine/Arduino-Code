///////////////////////////////////////////////
////////////// URINE PASSTHROUGH //////////////
///////////////////////////////////////////////
//              BY TAYLOR ROAN               //
//      October, 2016 for GoForward, Inc.    //
//       Powered by Arduino Nano Board       //
//                                           //
// Meant to operate an automatic urine       //
// passthrough window with:                  //
//   * an occupancy sensor to determine      //
//     whether a user in the restroom is at  //
//     the window                            //
//   * an IR proximity sensor to determine   //
//     if anything is inside the passthrough //
//   * an LED ring to illuminate the         //
//     passthrough                           //
//   * two limit switches at either extent   //
//     of the motion to determine proper     //
//     positioning                           //
//   * a manual override button to rotate    //
//     the passtrhough when needed           //
//   * a DC motor to rotate the passthough   //
//     powered via an L298N driver board     //
///////////////////////////////////////////////
///////////////////////////////////////////////


///////////////////////////////////////////////
////////////// THE PRE-GAME SHOW //////////////
///////////////////////////////////////////////

/****** COMMUNICATION ******/
//#include <stdio.h>

/****** SENSORS ******/
// URINE CUP //
// Sharp GP2Y0A21YK0F IR proximity sensor
#include <SharpIR.h>
#define cup_pin A0 // pin to the output of the Sharp IR rangefinder 
#define ir_mean 50 // (25) number of readings the library will make before calculating a mean distance 
#define ir_diff 90 // (93) the difference between two consecutive measurements to be taken as valid
#define model 1080  // "working distance" int that determines your sensor, 1080 for GP2Y0A21Y, 20150 for GP2Y0A02Y
SharpIR sharp(cup_pin, ir_mean, ir_diff, model);
char cup_state = 'L'; // 'E'mpty cup, 'U'ser has cup, 'F'ull cup, 'L'ab has cup
bool cup = false; // There is a cup in the passthrough

// OCCUPANCY //
// Sensky BS010l
#define occupancy_pin 12 // Pin to occupancy sensor output
bool alone = false; // No user is there
bool alone_state = false;  // No user last time you checked

// POSITIONING //
// Limit switches
#define lab_pin 7 // Pin to lab position limit switch (PIN 2 DOESN'T SEEM TO WORK!)
#define bathroom_pin 8 // Pin to bathroom position limit switch (PIN 2 DOESN'T SEEM TO WORK!)
bool toward_lab = false; // State of lab position limit switch
bool toward_bathroom = false; // State of bathroom position limit switch

// OVERRIDE //
#define override_pin 6 // Pin to lab position limit switch (PIN 2 DOESN'T SEEM TO WORK!)
bool override = false; // Whether or not we detect an override
// ^^ I should figure out how to make this an interrupt...

/****** MOTION ******/
// MOTORS //
#define cw_motor_pin 11 // The pin to drive the motor clockwise
#define ccw_motor_pin 10 // The pin to drive the motor counter clockwise
int rate = 20; // 0 is stop, from slowest 15 -> 255 for full speed
int move_time = 0; // ms
//int pause = 5000; // ms
int current_position = 'C'; // can be "L" (lab), "B" (bathroom), "T" (transition), or C (confused)
char goal = 'L'; // can be "L" (lab) or "B" (bathroom)

/****** LIGHTING ******/
// LEDs //
#define led_pin 9 // Pin to LED power 
int brightness = 255; // LED brightness value - I think between 0 -> 255


///////////////////////////////////////////////
/////////////////// WARM-UP ///////////////////
///////////////////////////////////////////////

void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps

  // Enable sensors
  pinMode(cup_pin, INPUT); // Enable the cup sensing IR rangefinder 
  pinMode(occupancy_pin, INPUT); // Enable the occupancy sensor 
  pinMode(A6, INPUT); // Enable the occupancy sensor

  // Enable buttons & switches
  pinMode(lab_pin, INPUT_PULLUP); // Enable the lab position limit switch
  pinMode(bathroom_pin, INPUT_PULLUP); // Enable the bathroom position limit switch
  pinMode(override_pin, INPUT_PULLUP); // Enable the manual override button

  // Enable LEDs 
  pinMode(led_pin, OUTPUT); // Enable the LED lighting
  analogWrite(led_pin,brightness); // Turn on LED lighting

  // Enable motor
  pinMode(cw_motor_pin, OUTPUT); // Enable the clockwise motor signal
  pinMode(ccw_motor_pin, OUTPUT); // Enable the counter clockwise motor signal 

  // All done!
  Serial.print("Setup Done!");
}


///////////////////////////////////////////////
/////////////////// IT'S ON! //////////////////
///////////////////////////////////////////////

void loop() {
  // INITIAL REPORT //
  // Timing
  Serial.print("TIME: ");
  Serial.print(millis());

  // SENSING //
  getSample();
  current_position = getPosition();
  goal = getGoal();

  // MOVING //
  go();

  Serial.print(". O:");
  Serial.print(digitalRead(override_pin)); // Returns "0" when pressed
  Serial.print(". L:");
  Serial.print(toward_lab); // Returns "0" when at the lab
  Serial.print(". B:");
  Serial.print(toward_bathroom); // Returns "0" when at the bathroom
  Serial.print(". O:");
  Serial.print(digitalRead(occupancy_pin));
  Serial.print(". C:");
  Serial.print(sharp.distance());

  Serial.println(". END LOOP."); // Let me know each time we end a loop and start a new line
}

///////////////////////////////////////////////
/////////////// GETTING POSITION //////////////
///////////////////////////////////////////////

int getPosition(){
  toward_lab = !digitalRead(lab_pin);
  toward_bathroom = !digitalRead(bathroom_pin);
  if(toward_lab){
    if(toward_bathroom){
      current_position = 'C';
      //Serial.print("CONFUSED - WHICH WAY AM I POINTING???!!!"); // Oh no.
    }else{
      current_position = 'L';
    }
  }else{
    if(toward_bathroom){
      current_position = 'B';
    }else{
      current_position = 'T';
    }
  }
  Serial.print(". @");
  Serial.print(char(current_position)); // Tell me which way you're facing and what you're doing
  return(current_position); // Report where you're at
}

///////////////////////////////////////////////
///////////////// GETTING GOAL ////////////////
///////////////////////////////////////////////

char getGoal(){
  alone = digitalRead(occupancy_pin);
  if(!alone){
    goal = 'B'; 
    Serial.print(". Hi, B");
  }
  
  override = !digitalRead(override_pin);
  if(override){
    if(toward_lab){
      goal = 'B'; 
      Serial.print(". O->B");
    }else if(toward_bathroom){
      goal = 'L'; 
      Serial.print(". O->L");
    }
  }
  Serial.print(". Goal: ");
  Serial.print(char(goal)); // Tell me where you wanna be
  return(goal); // Report where you're at
}

///////////////////////////////////////////////
//////////////////// MOVING ///////////////////
///////////////////////////////////////////////

void go(){
  if(current_position != goal){
    current_position = "T";
    Serial.print(". ->");

    switch(goal){
      
      case 'L':
      Serial.print("Lab!");
      analogWrite(cw_motor_pin,0);
      analogWrite(ccw_motor_pin,rate);
      break;
      
      case 'B':
      Serial.print("Bath!");
      analogWrite(cw_motor_pin,rate);
      analogWrite(ccw_motor_pin,0);
      break;
    }
  }else{
    Serial.print(". STOP");
    analogWrite(cw_motor_pin,0);
    analogWrite(ccw_motor_pin,0);
  }
}


  
///////////////////////////////////////////////
///////////////// GARBAGE DUMP ////////////////
///////////////////////////////////////////////

  // MOVING //
/*  analogWrite(cw_motor_pin,0);
  analogWrite(ccw_motor_pin,0);
  Serial.print(" [0 0] "); 
  delay(move_time);  
  analogWrite(cw_motor_pin,rate);
  analogWrite(ccw_motor_pin,0);  
  Serial.print(" [1 0] "); 
  delay(move_time);
  analogWrite(cw_motor_pin,0);
  analogWrite(ccw_motor_pin,rate);
  Serial.print(" [0 1] "); 
  delay(move_time);
  analogWrite(cw_motor_pin,rate);
  analogWrite(ccw_motor_pin,rate);
  Serial.println(" [1 1] "); 
  delay(move_time);
*/

// STATE MACHINE BASED ON OCCUPANCY //
 /* alone = digitalRead(cup);
  Serial.println(alone);
  if( alone == 1){
    if(alone_state == 0){
      delay(close_stop_time);
      alone_state = alone;
    }
    turn(lab_angle);
  }else{
    if(alone_state == 1){
      //delay(open_stop_time);
      alone_state = alone;
    }
    turn(potty_angle);
  }


 // TRYING TO MOVE SMARTLY //
// Turn towards the lab
void turn(float goal) {
  delay(slowness);
  
  float max_rate = (goal-pos)/4;
  rate+= (goal-pos)/abs(goal-pos);
  if((rate/max_rate > 1)){
    rate = max_rate;
  }
  if((pos-swing/2)/(goal-swing/2)>=.99){
    rate = 0;
    pos = goal;
    Serial.println("MADE IT!!!");
    //delay(stop_time);
  }
  pos +=rate;
  
  angle.write(pos);
  Serial.print("Position:  ");
  Serial.print(pos);
  Serial.print(".  Rate:   ");
  Serial.print(rate);
  Serial.print(".  Goal:   ");
  Serial.println(goal);  
}


*/

