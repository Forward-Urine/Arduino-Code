float value = 0;
float average_length = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);      // open the serial port at 9600 bps
  pinMode (A6, INPUT);
}

void loop() {
  Serial.print("Digital: ");
  Serial.print(digitalRead(A6));
  value = (analogRead(A6)+value*(average_length-1))/average_length;
  Serial.print("  Analog: ");
  Serial.print(analogRead(A6));
  Serial.print("  Analog Averaged: ");
  Serial.println(value);
}
