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
float time;
float last_time;

//declare pin aliases
int solenoid = 13;
int sensors = 0;

//control values
bool brake = false;
float seaLevelPressure = 1016;

void setup() {
  pinMode(solenoid, OUTPUT);
  Serial.begin(9600);
}

void loop() {
    //TODO: convert serial out to send over xbee
  sensors_event_t accel_event;
  sensors_event_t mag_event;
  sensors_event_t bmp_event;
  sensors_vec_t   orientation;
  
  accel.getEvent(&accel_event);
  mag.getEvent(&mag_event);
  
  if (dof.fusionGetOrientation(&accel_event, &mag_event, &orientation)) {
    /* 'orientation' should have valid .roll and .pitch fields */
    Serial.print(F("Orientation: "));
    Serial.print(orientation.roll);
    Serial.print(F(" "));
    Serial.print(orientation.pitch);
    Serial.print(F(" "));
    Serial.print(orientation.heading);
    Serial.println(F(""));
  }
  
  /* Calculate the altitude using the barometric pressure sensor */
  bmp.getEvent(&bmp_event);
  if (bmp_event.pressure){
    /* Get ambient temperature in C */
    float temperature;
    bmp.getTemperature(&temperature);
    /* Convert atmospheric pressure, SLP and temp to altitude */
    Serial.print(F("Alt: "));
    Serial.print(bmp.pressureToAltitude(seaLevelPressure,
                                        bmp_event.pressure,
                                        temperature)); 
    Serial.println(F(""));
    /* Display the temperature */
    Serial.print(F("Temp: "));
    Serial.print(temperature);
    Serial.println(F(""));
  }
    
  Wire.requestFrom(2,7);  //check I2C bit length
  
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
  
  delay(100);
}
