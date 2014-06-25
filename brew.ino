// https://github.com/br3ttb/Arduino-PID-Library
#include "PID.h"

// https://github.com/krvarma/Dallas_DS18B20_SparkCore
#include "DS18B20.h"
#include "OneWire.h"


// Input pin where DS18B20 is connected
int ds18b20Pin = D1;

// Output pin where relay is connected
int relayPin = A6;


// SparkCloud variable outputs
char tempInfo[64];
char pwmInfo[64];

// Temperature device on the D1 pin
DS18B20 ds18b20 = DS18B20(ds18b20Pin);

// PID: variables
double pointTemperature, actualTemperature, pidOutput;

// PID: Specify the links and initial tuning parameters
PID myPID(&actualTemperature, &pidOutput, &pointTemperature, 2, 5, 1, DIRECT);

// Timestamp of the last status events
unsigned long lastEventTimestamp;

 
 // Setup
void setup() {
    
    // Start serial on USB
    Serial.begin(9600);
    
    // SparkCloud: variables
    Spark.variable("temperature", &tempInfo, STRING);
    Spark.variable("pwm", &pwmInfo, STRING);
    
    // SparkCloud: functions
    Spark.function("setPoint", setPointTemperature);
    
    // Set relay pin mode to output
    pinMode(relayPin, OUTPUT);
    
    // Point temperature default
    pointTemperature = 5;

    //turn the PID on
    myPID.SetMode(AUTOMATIC);
    
    // Last event timestamp
    lastEventTimestamp = 0;
}
 
 // Loop
void loop() {
    
    // Searching for the ds18b20 device
    if(!ds18b20.search()){
        
        // Log to the serial
        Serial.println("No more addresses.");
        Serial.println();
        
        // Turn off the relay during the scan
        digitalWrite(relayPin, LOW);
        
        // Restart search
        ds18b20.resetsearch();
        delay(250);
        
        return;       
    }
    
    // Actual temperature in celsius
    actualTemperature = (double) ds18b20.getTemperature();
    
    // Calculate PID output
    myPID.Compute();
  
    
    // Send event to the SparkCloud
    if (millis() - lastEventTimestamp >= 1000) {
        
        // Convert values for output
        // convert pidOutput to 0-100
        sprintf(tempInfo, "%2.2f", actualTemperature);
        sprintf(pwmInfo, " %2.2f", (pidOutput / 255) * 100);
        
        // Log to the serial
        Serial.println(tempInfo);
        Serial.println(pwmInfo);
        
        // SparkCore: publish
        Spark.publish("tempInfo", tempInfo);
        Spark.publish("pwmInfo", pwmInfo);
        
        // Save timestamp of the event
        lastEventTimestamp = millis();
    }
  
    // Control relay
    analogWrite(relayPin, pidOutput);
}


// set point temperature
int setPointTemperature(String pointTemperatureStr)
{
    char info[64];
    
    // Convert to double
    pointTemperature = (double) atoi(pointTemperatureStr.c_str());
    
    // Log to the serial
    sprintf(info, "Point is set to: %2.2f", pointTemperature);
    Serial.println(info);
    
    return 1;
}
