#include "Adafruit_SHTC3.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
#define I2C_SDA 21
#define I2C_SCL 22

TwoWire I2CBME = TwoWire(0);
//hello

void setup() {
  Serial.begin(115200);
  //I2CBME.begin(I2C_SDA, I2C_SCL, 100000);

  while (!Serial)
    delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("SHTC3 test");
  if (! shtc3.begin()) {
    Serial.println("Couldn't find SHTC3");
    while (1) delay(1);
  }
  Serial.println("Found SHTC3 sensor");

}


void loop() {
  sensors_event_t humidity, temp;
  
  shtc3.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  delay(1000);
}
