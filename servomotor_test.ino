//#include <digitalio.h>
int power = 0; // Motor power (up to 255)
int torque = 0; // Motor power (up to 255)
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);      // open the serial port at 9600 bps
  analogReference(DEFAULT);
  pinMode(3,OUTPUT);
  pinMode(2,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
analogWrite(3, power);
torque = analogRead(2);
Serial.print("Commanded: ");
Serial.print(power);
Serial.print(".  Measured: ");
Serial.println(torque);
power += 1;
if(power>=256){
  Serial.println("FULL SPEED AHEAD!");
  delay(5000);
  power=0;
}
/*if(torque>=210){
  analogWrite(3, 0);  // STOP!
  Serial.println("DANGER!!!");
  delay(1000);
  //power=0;
}*/
//delay (50);
}
