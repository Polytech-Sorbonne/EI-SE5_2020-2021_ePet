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

#define NB_SECOND 60 
#define SeuilMin 37 //tests sur un mouvement sur 5 cm
#define SeuilMax 2.5

// Température
int t;

//tableaux des températures sur 15 minutes
int temp_tab[7];

//Accelerometre
#define SEUILX 2.5 //tests sur un mouvement sur 5 cm
#define SEUILY 2.5
double x_0;
double x_1;
double y_0;
double y_1;

double diffX = 0;
double diffY = 0;
int countX = 0 ,countY = 0;
int countmintX = 0, countmintY = 0;
int addedcountX = 0, addedcountY = 0; 

//tableaux des mouvements sur les deux axes sur 15 minutes 
int mouvX[15];
int mouvY[15];

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
#define I2C_SDA 21
#define I2C_SCL 22

TwoWire I2CBME = TwoWire(0);

Adafruit_MMA8451 mma = Adafruit_MMA8451();


hw_timer_t * timer;
volatile int interruptCounter;
volatile int minutes;
int totalInterruptCounter;
sensors_event_t humidity, temp;
sensors_event_t event;

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
    
  }
  Serial.println("Found SHTC3 sensor");
  

  //accelerometre
   if (! mma.begin()) {
    Serial.println("Couldnt start");
    
  }
  Serial.println("MMA8451 found!");

  // Range 2G, 4G, 8G
  mma.setRange(MMA8451_RANGE_2_G);

  // getRange() -> 2G = 1, 4G = 2, 8G = 3
  Serial.print("Range = "); Serial.print(2 << mma.getRange());  
  Serial.println("G");

    //initialisation des données d'acceleration
  mma.getEvent(&event);
  x_0 = event.acceleration.x;
  y_0 = event.acceleration.y;

  Serial.print("init X0 : ");Serial.println(x_0);
  Serial.print("init Y0 : ");Serial.println(y_0);
  shtc3.getEvent(&humidity, &temp); // populate temp object with fresh data



  
  xTaskCreate(taskOne,"taskOne",1000, NULL, 1,NULL);

}


void loop() {
  if (interruptCounter > 0) {
    interruptCounter--;
    totalInterruptCounter++;
    shtc3.getEvent(&humidity, &temp); // populate temp object with fresh data
  }
}

void taskOne(void* parameter){
;  while(1){
    mma.getEvent(&event);
    //Serial.println("temperature");
    Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
    float tmp = temp.temperature;
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);
    t = convFloatToInt(tmp);
    temp_tab[totalInterruptCounter] = t;
    if (totalInterruptCounter == 6) {
      for (int i = 0; i < 7; i++) {
        Serial.print("Température n° "); Serial.print(i); Serial.print(": temp : "); Serial.println(temp_tab[i]);
      }
      totalInterruptCounter = 0;
    }
  //accelero 
  //accelerometre
    Serial.println("Accelero mesure");
     mma.getEvent(&event);
      mma.read();
      x_1 = event.acceleration.x;
      y_1 = event.acceleration.y;
      Serial.print("x_1 : ");Serial.println(x_1);
      Serial.print("y_1 : ");Serial.println(y_1);
      diffX = x_1 - x_0;
      diffY = y_1 - y_0;
      if (abs(diffX) > SEUILX)
      {
        countX ++;
        Serial.println("Mouvement sur X detecte");
        mouvX[minutes%15] = 1; 
      } 
      else
      {
        mouvX[minutes%15] = 0; 
      }
      if (abs(diffY) > SEUILY)
      {
        countY ++;
        Serial.println("Mouvement sur Y detecte");
        mouvY[minutes%15] = 1; 
      } 
      else
      {
        mouvY[minutes%15] = 0; 
      }
      x_0 = x_1;
      y_0 = y_1;
    
    vTaskDelay(60000);
  }
}
