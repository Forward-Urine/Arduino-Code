/****** DETECTION ******/
// URINE CUP //
// Sharp GP2Y0A21YK0F IR proximity sensor
#include <SharpIR.h>
#define cup_pin A0 // pin to the output of the Sharp IR rangefinder 
#define ir_mean 500 // (25) number of readings the library will make before calculating a mean distance 
#define ir_diff 90 // (93) the difference between two consecutive measurements to be taken as valid
#define model 1080  // "working distance" int that determines your sensor, 1080 for GP2Y0A21Y, 20150 for GP2Y0A02Y
SharpIR sharp(cup_pin, ir_mean, ir_diff, model);

// OCCUPANCY //
// Sensky BS010l
#define occupancy_pin 12 // Pin to occupancy sensor output
bool alone = false; // Whether or not a user is there
bool alone_state = false;  // If the user was there last time you checked

/****** MOTION ******/
// FOR SERVOS
#include <Servo.h>
Servo angle;
#define motor_pin 11 // The pin to the motor
#define close_limit_pin 3 // The pin to the close limit switch (PIN 2 DOESN'T SEEM TO WORK!)
int rate = 20; // 0 is stop, from slowest 15 -> 255 for full speed
int move_time = 500; // ms
//int pause = 5000; // ms
char current_position = "lab"; // can be "lab" or "bathrooom" or "transit" if between states
char goal_position = "lab"; // can be "lab" or "bathrooom" 

void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps
  pinMode(cup_pin, INPUT); // Enable the cup sensing IR rangefinder 
  pinMode(occupancy_pin, INPUT); // Enable the occupancy sensor 
  pinMode(A6, INPUT); // Enable the occupancy sensor
  
  pinMode(motor_pin, OUTPUT); // Enable the motor 
  pinMode(close_limit_pin, INPUT_PULLUP); // Enable the close limit switch

  Serial.print("Setup Done!");
}

void loop() {
  Serial.print("CLOSE LIMIT: ");
  Serial.print(digitalRead(close_limit_pin)); // Returns "0" when closed
  Serial.print(". CUP: ");
  Serial.print(sharp.distance());
  Serial.print(". OCCUPANCY: ");
  Serial.println(digitalRead(occupancy_pin));
  analogWrite(motor_pin,0);
  delay(move_time);
  analogWrite(motor_pin,rate);
  delay(move_time);
  
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
  
  /*
  // Accelerate
  for (pos = lab_angle; pos <= lab_angle+swing/2; pos+= (pos-lab_angle)/10+1){
  delay(slowness);
  angle.write(pos);
  Serial.println(pos);
  Serial.print("Accelerating...   ");
  }
  
  // Decelerate
  for (pos = lab_angle+swing/2; pos <= potty_angle; pos+= (potty_angle-pos)/20+1){
  delay(slowness);
  angle.write(pos);
  Serial.println(pos);
  Serial.print("Decelerating...   ");
  }
  Serial.println("OPEN!!   ");
  delay(pause);
  
  // Accelerate
  for (pos = potty_angle; pos >= lab_angle+swing/2; pos+= (pos-potty_angle)/10-1){
  delay(slowness);
  angle.write(pos);
  Serial.println(pos);
  Serial.print("Accelerating...   ");
  }
  
  // Decelerate
  for (pos = lab_angle+swing/2; pos >= lab_angle; pos+= (lab_angle-pos)/10-1){
  delay(slowness);
  angle.write(pos);
  Serial.println(pos);
  Serial.print("Decelerating...   ");
  }
  Serial.print("CLOSED!!   ");
  delay(pause);
  
  /*delay(position_hold_time);
  angle.write(potty_angle);
  Serial.println(potty_angle);
  Serial.println("potty_angle!");
    
  delay(position_hold_time);
  angle.write(lab_angle);
  Serial.println(lab_angle);
  Serial.println("lab_angle!");*/
}
/* GARBAGE DUMP
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

