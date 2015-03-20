#include <Wire.h>

//timing values
float time;
float last_time;

//declare pin aliases
int solenoid = 13;
int sensors = 0;

//control values
bool brake = false;

void setup() {
  pinMode(solenoid, OUTPUT);
  
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  //get the sensor data
  Wire.requestFrom(2,7);
  
  while(Wire.available()){
    char c = Wire.read();
    Serial.print(c);
  }
  
  //math goes here
  
  if(brake == false){
    digitalWrite(solenoid, LOW);
  } else {
    digitalWrite(solenoid, HIGH);
  }
}
