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

/****** SENSORS ******/
// URINE CUP //
// Sharp GP2Y0A21YK0F IR proximity sensor
#include <SharpIR.h>
#define cup_pin A0 // pin to the output of the Sharp IR rangefinder 
#define ir_mean 50 // (25) number of readings the library will make before calculating a mean distance 
#define ir_diff 90 // (93) the difference between two consecutive measurements to be taken as valid
#define model 1080  // "working distance" int that determines your sensor, 1080 for GP2Y0A21Y, 20150 for GP2Y0A02Y
SharpIR sharp(cup_pin, ir_mean, ir_diff, model);
int floor_cutoff = 25; // [cm] distance from sensor to bottom of passthrough (which it should see when no cup present
int ir_distance = floor_cutoff; // [cm] distance from sensor to what it sees
int cup_cutoff = 20; // [cm] distance from sensor under which a cup is believed to be present
char cup_state = 'N'; // 'N'ew cup, 'U'ser has cup, 'F'ull cup, 'L'ab has cup
bool cup = false; // There is a cup in the passthrough

// OCCUPANCY //
// Sensky BS010l
#define occupancy_pin 12 // Pin to occupancy sensor output
bool user = false; // No user is there
bool user_state = false;  // No user last time you checked

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
int rate = 255; // 0 is stop, from slowest 15 -> 255 for full speed
int move_time = 0; // ms
int current_position = 'C'; // can be "L" (lab), "B" (bathroom), "T" (transition), or C (confused)
char goal = 'L'; // can be "L" (lab) or "B" (bathroom)
char mission = goal; // when the goal changes, make sure you mean it
int hesitation = 4000; // [ms] hesitate this much to decide you really want to change your goal

/****** LIGHTING ******/
// LEDs //
#define led_pin 9 // Pin to LED power 
int brightness = 255; // LED brightness value - I think between 0 -> 255

/****** COMMUNICATION & TIMING ******/
//#include <stdio.h>
int current_time = 0;
int change_time = 0; // when did we last think about changing our mission?

///////////////////////////////////////////////
/////////////////// WARM-UP ///////////////////
///////////////////////////////////////////////

void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps

  // Enable sensors
  pinMode(cup_pin, INPUT); // Enable the cup sensing IR rangefinder
  pinMode(occupancy_pin, INPUT); // Enable the occupancy sensor
  //pinMode(A6, INPUT); // Enable the occupancy sensor

  // Enable buttons & switches
  pinMode(lab_pin, INPUT_PULLUP); // Enable the lab position limit switch
  pinMode(bathroom_pin, INPUT_PULLUP); // Enable the bathroom position limit switch
  pinMode(override_pin, INPUT_PULLUP); // Enable the manual override button

  // Enable LEDs
  pinMode(led_pin, OUTPUT); // Enable the LED lighting
  analogWrite(led_pin, brightness); // Turn on LED lighting

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
  current_time = millis();
  Serial.print("TIME: ");
  Serial.print(current_time);

  // SENSING //
  current_position = checkPosition();
  cup_state = checkCup();
  goal = checkGoal();

  // MOVING //
  if (goal == mission) {
    change_time = current_time;
  } else if (current_time - change_time > hesitation) {
    mission = goal;
  }
  go();

  //Serial.print(". O:");
  //Serial.print(digitalRead(override_pin)); // Returns "0" when pressed
  Serial.print(". L:");
  Serial.print(toward_lab); // Returns "0" when at the lab
  Serial.print(". B:");
  Serial.print(toward_bathroom); // Returns "0" when at the bathroom
  Serial.print(". O:");
  Serial.print(digitalRead(occupancy_pin));
  Serial.print(". C:");
  Serial.print(sharp.distance());
  Serial.println(".");
  //Serial.println(". END."); // Let me know each time we end a loop and start a new line
}

///////////////////////////////////////////////
//////////////// CHECK POSITION ///////////////
///////////////////////////////////////////////

int checkPosition() {
  toward_lab = !digitalRead(lab_pin);
  toward_bathroom = !digitalRead(bathroom_pin);
  if (toward_lab) {
    if (toward_bathroom) {
      current_position = 'C';
      //Serial.print("CONFUSED - WHICH WAY AM I POINTING???!!!"); // Oh no.
    } else {
      current_position = 'L';
    }
  } else {
    if (toward_bathroom) {
      current_position = 'B';
    } else {
      current_position = 'T';
    }
  }
  Serial.print(". @");
  Serial.print(char(current_position)); // Tell me which way you're facing and what you're doing
  return (current_position); // Report where you're at
}

///////////////////////////////////////////////
/////////////////// CHECK CUP /////////////////
///////////////////////////////////////////////

char checkCup() { // 'N' cup, 'U'ser has cup, 'F'ull cup, 'L'ab has cup
  ir_distance = sharp.distance();
  if (ir_distance <= cup_cutoff && ir_distance > 0) {
    cup = true;
  } else if (ir_distance >= cup_cutoff && ir_distance <= floor_cutoff) {
    cup = false;
  } else {
    Serial.print(". BAD CUP");
  }

  if (current_position == 'B') { // if passthrough is facing the bathroom...
    if (cup == false) { // user has the cup if it isn't there
      cup_state = 'U';
    } else {
      if (cup_state == 'N') { // if the cup hasn't been removed it is still new
        cup_state = 'N';
      } else { // if the cup is there, but it isn't new, than it is full (or needs to be replaced)
        cup_state = 'F';
      }
    }
  } else if (current_position == 'L') { // if passthrough is facing the lab...
    if (cup == false) { // lab has the cup if it isn't there
      cup_state = 'L';
    } else {
      switch (cup_state) {
        case 'L': // if there is a cup now, but wasn't last time, it is new
          cup_state = 'N';
          break;
        case 'N': // if the cup is there, and it is new, then it is still new
          cup_state = 'N';
          break;
        case 'F': // if the cup is there, but it isn't new, then it is full (or needs to be replaced)
          cup_state = 'F';
          break;
      }
    }
  } // ignore transition & confusion because nothing should be happening anyway

  Serial.print(". Cup:");
  Serial.print(char(cup_state)); // Tell me the state of the cup
  Serial.print("@: ");
  Serial.print(ir_distance); // Tell me how far rangefinder can see
  Serial.print("mm");
  return (cup_state); // Report if you have the cup
}

///////////////////////////////////////////////
////////////////// CHECK GOAL /////////////////
///////////////////////////////////////////////

char checkGoal() {
  user = digitalRead(occupancy_pin);

  switch (cup_state) {
    case 'U': // If the user has the cup, keep the passthrough open to the bathroom
      goal = 'B';
      break;
    case 'L': // If the cup is in the lab, keep the passthrough open to the lab
      goal = 'L';
      break;
    case 'F': // If the cup full, send it to the lab if the user isn't there
      if (user && goal == 'B') {
        goal = 'B';
      } else {
        goal = 'L';
      }
      break;
    case 'N': // If the cup is new, send it to the user when they arrive
      if (goal == 'B') {
        goal = 'B';
      } else if (user) {
        goal = 'B';
      } else {
        goal = 'L';
      }
      break;
  }

  override = !digitalRead(override_pin);
  if (override) {
    if (toward_lab) {
      goal = 'B';
      Serial.print(". O->B");
    } else if (toward_bathroom) {
      goal = 'L';
      Serial.print(". O->L");
    }
  }
  Serial.print(". Goal: ");
  Serial.print(char(goal)); // Tell me where you wanna be
  return (goal); // Report where you're at
}

///////////////////////////////////////////////
//////////////////// MOVING ///////////////////
///////////////////////////////////////////////

void go() {
  if (current_position != mission) {
    current_position = 'T';
    Serial.print(". ->");

    switch (mission) {
      case 'L':
        Serial.print("Lab!");
        analogWrite(cw_motor_pin, 0);
        analogWrite(ccw_motor_pin, rate);
        break;
      case 'B':
        Serial.print("Bath!");
        analogWrite(cw_motor_pin, rate);
        analogWrite(ccw_motor_pin, 0);
        break;
    }
  } else {
    Serial.print(". STOP");
    analogWrite(cw_motor_pin, 0);
    analogWrite(ccw_motor_pin, 0);
  }
  Serial.print(". Mission: ");
  Serial.print(char(mission)); // Tell me where you're headed
}



///////////////////////////////////////////////
///////////////// GARBAGE DUMP ////////////////
///////////////////////////////////////////////

