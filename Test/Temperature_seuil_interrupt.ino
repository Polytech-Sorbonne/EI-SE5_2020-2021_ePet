#include <Wire.h>
#include <Arduino.h>
#ifdef ARDUINO_ARCH_SAMD
#include <WiFi101.h>
#elif defined ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#elif defined ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#error Wrong platform
#endif

#include <WifiLocation.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

//Librairies Température
#include "Adafruit_SHTC3.h"

#define SeuilMin 37 //tests sur un mouvement sur 5 cm
#define SeuilMax 2.5
#define NB_SECOND 60 //75 // 1 min 15

// Température
int t;

//tableaux des températures sur 15 minutes
int temp_tab[7];

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
#define I2C_SDA 21
#define I2C_SCL 22

TwoWire I2CBME = TwoWire(0);

hw_timer_t * timer;
volatile int interruptCounter;
int totalInterruptCounter;
sensors_event_t humidity, temp;

//Conversion d'un float en int
int convFloatToInt(float x) {
  int integer = x * 100;
  Serial.print("Integer : ");
  Serial.println(integer);
  return integer ;
}

// timer routine
void IRAM_ATTR onTimer() {
  interruptCounter++;
}

void setup() {
  Serial.begin(115200);
  //creation du timer
  xTaskCreate(taskOne,"taskOne",1000, NULL, 1,NULL);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, NB_SECOND * 1000000, true);   // 1 000 000 ms = 1 s
  timerAlarmEnable(timer);

  //desactiver bluetooth et wifi
  WiFi.mode(WIFI_OFF);
  btStop();
  Serial.println("SHTC3 test");
  if (! shtc3.begin()) {
    Serial.println("Couldn't find SHTC3");
    while (1) delay(1);
  }
  Serial.println("Found SHTC3 sensor");
}


void loop() {
  if (interruptCounter > 0) {
    interruptCounter--;
    totalInterruptCounter++;
    shtc3.getEvent(&humidity, &temp); // populate temp object with fresh data
  }
}

void taskOne(void* parameter){
  while(1){
    //Serial.println("temperature");
    Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
    float tmp = temp.temperature;
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);
    t = convFloatToInt(tmp);
    temp_tab[totalInterruptCounter - 2] = t;
    if (totalInterruptCounter == 8) {
      for (int i = 0; i < 7; i++) {
        Serial.print("Température n° "); Serial.print(i); Serial.print(": temp : "); Serial.println(temp_tab[i]);
      }
      totalInterruptCounter = 0;
    }
    vTaskDelay(60000);
  }
}
