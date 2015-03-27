#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_10DOF.h>

//Assign IDs to the sensors
Adafruit_10DOF                dof   = Adafruit_10DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);

//timing values
unsigned long time = 0;
unsigned long last_time = 0;
unsigned long time_difference;
unsigned long coast_time = 0;
unsigned long flight_time = 5000;  //  ** SET THIS BEFORE LAUNCH ** time from burnout to appogee in ms
int target_altitude = 1625;

//declare pin aliases
int solenoid = 13;
int sensors = 0;

//control values
bool brake = false;
float seaLevelPressure = 1016;  // ** SET THIS BEFORE LAUNCH **
float startAltitude = 0;
float targetAltitude;
float altitude;
float last_altitude;
float altitude_difference;
float estimated_altitude;
float last_difference;
float velocity;

void setup() {
  pinMode(solenoid, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  sensors_event_t bmp_event;

  // Calculate the altitude using the barometric pressure sensor
  bmp.getEvent(&bmp_event);
  if (bmp_event.pressure){
    float temperature;
    bmp.getTemperature(&temperature);
    altitude = bmp.pressureToAltitude(seaLevelPressure, bmp_event.pressure, temperature);
    
    if(startAltitude == 0){
      startAltitude = altitude;
      targetAltitude = startAltitude + 1625;
    }
  }
  
  //calculate speed
  if(last_time == 0){
    last_time = millis();
  } else {
    time = millis();
    time_difference = time - last_time;
    altitude_difference = last_altitude - altitude;
    velocity = altitude_difference / time_difference;
    last_time = time;
    last_altitude = altitude;
    last_difference = altitude_difference;
  }
  
  if(altitude_difference < last_difference + 0.5){  //the rocket is slowing down (end powered flight)
    if(coast_time == 0){
      coast_time = millis();
    }
    
    estimated_altitude = altitude + (velocity * (flight_time - coast_time)) + ( 4.9 * (flight_time - coast_time) * (flight_time - coast_time));
  
    if(estimated_altitude > targetAltitude){  // The rocket is going to fast
      brake = true;
    } else {
      brake = false;
    }
  }
  
  if(brake == false){
    digitalWrite(solenoid, LOW);
  } else {
    digitalWrite(solenoid, HIGH);
  }
  
  delay(100);
}
