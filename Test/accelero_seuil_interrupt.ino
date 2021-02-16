/**************************************************************************/
/*!
    @file     Adafruit_MMA8451.h
    @author   K. Townsend (Adafruit Industries)
    @license  BSD (see license.txt)
    This is an example for the Adafruit MMA8451 Accel breakout board
    ----> https://www.adafruit.com/products/2019
    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!
    @section  HISTORY
    v1.0  - First release
*/
/**************************************************************************/

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
#define SEUILX 2.5 //tests sur un mouvement sur 5 cm
#define SEUILY 2.5
//#define TOUR_MINUTES 6000/15
#define NB_SECOND 60

double x_0;
double x_1;
double y_0;
double y_1;

//differences maximales selon les axes
double max_diffX = 0;
double max_diffY = 0;

//tableaux de mouvement sur 15 minutes
int mouv_tabX[15];
int mouv_tabY[15];

double diffX = 0;
double diffY = 0;
int countX = 0 ,countY = 0;
int countmintX = 0, countmintY = 0;
int addedcountX = 0, addedcountY = 0; 
bool significant = false;

//volatile int minutes = 0; // compteur de minutes

Adafruit_MMA8451 mma = Adafruit_MMA8451();

hw_timer_t * timer;
volatile int interruptCounter;
int totalInterruptCounter;
//portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; // ???
 
//SemaphoreHandle_t barrierSemaphore = xSemaphoreCreateCounting( 1, 0 ); // ???


// timer routine
void IRAM_ATTR accelero(){
  Serial.println("accelero");  
  sensors_event_t event; 
  mma.getEvent(&event);
  mma.read();
  x_1 = event.acceleration.x;
  y_1 = event.acceleration.y;
  diffX = x_1 - x_0;
  diffY = y_1 - y_0;
      
  if (abs(diffX) > max_diffX)
    max_diffX = diffX;
  if (abs(diffX) > SEUILX){
    countX ++;
    Serial.println("Mouvement sur X detecte");
  } 
  if (abs(diffY) > max_diffY)
    max_diffY = diffY;
  if (abs(diffY) > SEUILY){
    countY ++;
    Serial.println("Mouvement sur Y detecte");
  }     
  x_0 = x_1;
  y_0 = y_1;
  delay(10); 
  interruptCounter++;
}

void setup() {   
  Serial.begin(115200);   
  //creation du timer
  timer = timerBegin(0, 80, true);   
  timerAttachInterrupt(timer, &accelero, true);   
  timerAlarmWrite(timer, NB_SECOND * 1000000, true);   // 1 000 000 ms = 1 s
  timerAlarmEnable(timer);
  
  //desactiver bluetooth et wifi
  WiFi.mode(WIFI_OFF);
  btStop();
  Serial.println("Adafruit MMA8451 test!");
  
  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!");

  // Range 2G, 4G, 8G
  mma.setRange(MMA8451_RANGE_2_G);

  // getRange() -> 2G = 1, 4G = 2, 8G = 3
  Serial.print("Range = "); Serial.print(2 << mma.getRange());  
  Serial.println("G");
  //rattacher une routine au timer

  //initialisation des données d'acceleration
  sensors_event_t event; 
  mma.getEvent(&event);
  x_0 = event.acceleration.x;
  y_0 = event.acceleration.y;

  Serial.print("init X0 : ");Serial.println(x_0);
  Serial.print("init Y0 : ");Serial.println(y_0);
}

//Convertit des infomartions de mouvement en une chaîne de caractère
void convByteToCharAcc(int mouv_tab[15], char *x_trad){
  Serial.println("convBytetochar");
  String trad = "";
  for (int i = 0; i < 14; i++)
    trad.concat(String(mouv_tab[i+1],HEX)); // stocker les 14 dernières infos sur le mouvement sur X dans x_trad
  trad.toCharArray(x_trad, 14); 
  Serial.println(x_trad);
  
}

void loop() {

  if (interruptCounter > 0) {
 
    interruptCounter--;
    totalInterruptCounter++;
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);
    Serial.print("Nombre de minutes :");Serial.println(totalInterruptCounter);

    if (countX == 0)
      mouv_tabX[totalInterruptCounter - 1] = 0;
    else{
      countmintX ++;
      mouv_tabX[totalInterruptCounter - 1] = 1;
    }
    if (countY == 0)
      mouv_tabY[totalInterruptCounter - 1] = 0;
    else{
      countmintY ++;
      mouv_tabY[totalInterruptCounter - 1] = 1;
    }
    addedcountX += countX;
    addedcountY += countY;
    
    //remise à zero des compteurs
    countX = 0;
    countY = 0;
    
    //remise a zero des differences max : 
    max_diffX = 0;
    max_diffY = 0;
    
    //remise à zero des compteurs
    countX = 0;
    countY = 0;
    
    //remise à zero des compteurs globaux
    addedcountX = 0;
    addedcountY = 0;
    
    //remise à zero des compteurs de mouvement significants
    countmintX = 0;
    countmintY = 0;
        
    if (totalInterruptCounter == 15){ 
      for (int i = 0; i < 15; i++)
      {
        //X data
        if (mouv_tabX[i] == 0)
        {
          Serial.print("mouvement à la minute n° ");Serial.print(i);Serial.println(": X :NULL");
        }
        else {
          Serial.print("mouvement à la minute n° ");Serial.print(i);Serial.println(": X :YES ");
        }
        //Y data
        if (mouv_tabY[i] == 0)
        {
          Serial.print("mouvement à la minute n° ");Serial.print(i);Serial.println(": Y :NULL");
          Serial.println("");
        }
        else {
          Serial.print("mouvement à la minute n° ");Serial.print(i);Serial.println(": Y :YES ");
          Serial.println("");
        }
      }
      totalInterruptCounter = 0;
    }
  }
}
