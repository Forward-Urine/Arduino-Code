//FOR SERVOS
#include <Servo.h>

// Sensors
int proximity = 2; // The pin the proximite sensor output is connected to
bool alone = false; // Whether or not a user is there
bool alone_state = false;  // If the user was there last time you checked

// Servos
Servo angle;
int servo_pin = 3; // The pin the servo command is connected to
float lab_angle = 27; // Servo angle correspoinding to the lab
float potty_angle = lab_angle+91; // Servo angle correspoinding to the bathroom (up to 165)
float swing = potty_angle - lab_angle ; // How much the servo will swing
float pos = lab_angle; // Current servo position
float rate = 0; // Current servo rate
int slowness = 50; // ms
int open_stop_time = 0000; // ms
int close_stop_time = 0000; // ms
//int pause = 5000; // ms

void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps

  pinMode(proximity, INPUT); // Enable the proximity sensor
  
  angle.attach(servo_pin);
  angle.write(lab_angle);
  
  Serial.print("Setup Done!");
}

void loop() {
  alone = digitalRead(proximity);
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


