#include "ds18b20/DS18B20.h"
#include "ds18b20/OneWire.h"

DS18B20 ds18b20 = DS18B20(D2);
char szInfo[64];

 
 // Setup
void setup() {
    Serial1.begin(9600);
}
 
 // Loop
void loop() {
    if(!ds18b20.search()){
        Serial1.println("No more addresses.");
        Serial1.println();
        ds18b20.resetsearch();
        delay(250);
        
        return;       
    }
    
    float celsius = ds18b20.getTemperature();
  
    sprintf(szInfo, "{ temp: %2.2f, chip: \"%s\")", celsius, ds18b20.getChipName());
  
    Spark.publish("tmpinfo", szInfo);
  
    delay(1000);
}
