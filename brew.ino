// https://github.com/br3ttb/Arduino-PID-Library
#include "PID.h"

// https://github.com/krvarma/Dallas_DS18B20_SparkCore
#include "DS18B20.h"
#include "OneWire.h"

DS18B20 ds18b20 = DS18B20(D1);
unsigned long lastTempRequest = 0;
char szInfo[64];

int RelayPin = A6;

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,2,5,1, DIRECT);

 
 // Setup
void setup() {
    Serial1.begin(9600);
    
    Spark.variable("temperature", &szInfo, STRING);
    
    pinMode(RelayPin, OUTPUT);
    Input = 0;
    
    Setpoint = 40;

    //turn the PID on
    myPID.SetMode(AUTOMATIC);
}
 
 // Loop
void loop() {
    if(!ds18b20.search()){
        Serial1.println("No more addresses.");
        Serial1.println();
        ds18b20.resetsearch();
        delay(250);
        
        // Turn off
        digitalWrite(RelayPin, LOW);
        
        return;       
    }
    
    float celsius = ds18b20.getTemperature();
  
    sprintf(szInfo, "%2.2f - %2.2f", celsius, (Output / 255) * 100);
    
    if (millis() - lastTempRequest >= 2000) {
        Spark.publish("tmpinfo", szInfo);
        lastTempRequest = millis();
    }
    
    // PID
    Input = (double) celsius;
  
    myPID.Compute();
  
    analogWrite(RelayPin, Output);
}
