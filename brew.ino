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
char pointInfo[64];
char pwmInfo[64];

// Temperature device on the D1 pin
DS18B20 ds18b20 = DS18B20(ds18b20Pin);

// PID: variables
double pointTemperature, actualTemperature, pidOutput;

// PID: Specify the links and initial tuning parameters
PID myPID(&actualTemperature, &pidOutput, &pointTemperature, 25, 1000, 9, DIRECT);

// Timestamp of the last status events
unsigned long lastEventTimestamp;

 
 // Setup
void setup() {
    
    // Start serial on USB
    Serial.begin(9600);
    
    // SparkCloud: variables
    Spark.variable("temperature", &tempInfo, STRING);
    Spark.variable("point", &pointInfo, STRING);
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
    if(actualTemperature == 0 && !ds18b20.search()) {        
        
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
    
    
    // Send event to the SparkCloud
    if (millis() - lastEventTimestamp >= 1000) {

        // Actual temperature in celsius
        actualTemperature = ds18b20.getTemperature();

        // Compute PID
        if(myPID.Compute()) {
            
            // Control relay
            analogWrite(relayPin, pidOutput);
        }
        
        // Convert values for output
        // convert pidOutput to 0-100
        sprintf(tempInfo, "%2.2f", actualTemperature);
        sprintf(pointInfo, "%2.2f", pointTemperature);
        sprintf(pwmInfo, " %2.2f", (pidOutput / 255) * 100);
        
        // Log to the serial
        Serial.println("---");
        Serial.println(tempInfo);
        Serial.println(pwmInfo);
        Serial.println(pointInfo);
        
        // SparkCore: publish
        Spark.publish("tempInfo", tempInfo);
        Spark.publish("pointInfo", pointInfo);
        Spark.publish("pwmInfo", pwmInfo);
        
        // Save timestamp of the event
        lastEventTimestamp = millis();
    }
  
}


// set point temperature
int setPointTemperature(String command)
{
    Serial.println("setPointTemperature called");

    // Convert to double
    pointTemperature = (double) command.toInt();

    Serial.println("Point temperature set");
    Serial.println(pointTemperature);
    
    return 1;
}
