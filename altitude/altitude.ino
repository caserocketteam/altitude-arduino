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
int target_altitude = 1625;
int launch_altitude = 400;  // ** SET THIS BEFORE LAUNCH **
int absolute_distance = target_altitude + launch_altitude;

//declare pin aliases
int solenoid = 13;
int sensors = 0;

//control values
bool brake = false;
float seaLevelPressure = 1016;  // ** SET THIS BEFORE LAUNCH **
float altitude;
float last_altitude;
float altitude_change;

void setup() {
  pinMode(solenoid, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  sensors_event_t accel_event;
  sensors_event_t bmp_event;

  /* Calculate the altitude using the barometric pressure sensor */
  bmp.getEvent(&bmp_event);
  if (bmp_event.pressure){
    /* Get ambient temperature in C */
    float temperature;
    bmp.getTemperature(&temperature);
    
    /* Convert atmospheric pressure, SLP and temp to altitude */
    altitude = bmp.pressureToAltitude(seaLevelPressure, bmp_event.pressure, temperature); 
  }
    
  Wire.requestFrom(2,7);  //check I2C bit length
  
  while(Wire.available()){
    char c = Wire.read();
    Serial.print(c);
  }
  
  //calculate speed
  if(last_time == 0){
    last_time = millis();
  } else {
    time = millis();
    time_difference = time - last_time;
    accel.getEvent(&accel_event);
    
    last_time = time;
  }
  
  //calculate change in altitude
  altitude_change = altitude - last_altitude;
  
  //math for control goes here
  
  if(brake == false){
    digitalWrite(solenoid, LOW);
  } else {
    digitalWrite(solenoid, HIGH);
  }
  
  delay(100);
}
