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
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#define SEUIL 0.75 //tests sur un mouvement sur 5 cm

double x_0;
double x_1;
double y_0;
double y_1;
double max_diffX = 0;
double max_diffY = 0;

Adafruit_MMA8451 mma = Adafruit_MMA8451();

void setup(void) {
  Serial.begin(112500);
  
  Serial.println("Adafruit MMA8451 test!");
  

  if (! mma.begin()) {
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("MMA8451 found!");

  // Range 2G, 4G, 8G
  mma.setRange(MMA8451_RANGE_8_G);

  // getRange() -> 2G = 1, 4G = 2, 8G = 3
  Serial.print("Range = "); Serial.print(2 << mma.getRange());  
  Serial.println("G");
  
}

void loop() {
  double diffX = 0;
  double diffY = 0;
  double meanX = 0, meanY = 0;
  int countX = 0 ,countY = 0; 
  sensors_event_t event; 
  mma.getEvent(&event);
  
  Serial.println("Nouvelle mesure");
  // Read the 'raw' data in 14-bit counts
  mma.read();
  Serial.print("X:\t"); Serial.print(mma.x); 
  Serial.print("\tY:\t"); Serial.print(mma.y); 
  Serial.print("\tZ:\t"); Serial.print(mma.z); 
  Serial.println();
  x_0 = event.acceleration.x;
  y_0 = event.acceleration.y;
 
  //ol wait  : // delay(60000); // wait for 1 min
  /* Get a new sensor event */
  for (int i = 0; i < 6000; i++)
  {
    sensors_event_t event; 
    mma.getEvent(&event);
    mma.read();
    x_1 = event.acceleration.x;
    y_1 = event.acceleration.y;
    diffX = x_1 - x_0;
    diffY = y_1 - y_0;
    if (abs(diffX) > max_diffX)
    {
      max_diffX = diffX;
    } 
    if (abs(diffX) > SEUIL)
    {
      countX ++;
    } 
    if (abs(diffY) > max_diffY)
    {
      max_diffY = diffY;
    }
     if (abs(diffY) > SEUIL)
    {
      countY ++;
    } 
    meanX += abs(diffX);
    meanY += abs(diffY);
   
    x_0 = x_1;
    y_0 = y_1;

    delay(10);
     
  }
  mma.getEvent(&event);
  //mma.read();
  //Serial.print("X:\t"); Serial.print(mma.x); 
  //Serial.print("\tY:\t"); Serial.print(mma.y); 
  //Serial.print("\tZ:\t"); Serial.println(mma.z); 
  //x_1 = mma.x;
  //y_1 = mma.y;

  //Serial.print("La difference sur l'axe X est :");
  //Serial.println(x_1 - x_0);
  //Serial.print("La difference sur l'axe Y est :");
  //Serial.println(y_1 - y_0);
  Serial.println("--------------");
  Serial.print("La difference max sur X pendant une minute etait : ");Serial.println(max_diffX);
  Serial.print("La difference max sur Y pendant une minute etait : ");Serial.println(max_diffY);
  Serial.print("Nombre total de changements considerables sur X : ");Serial.println(countX);
  Serial.print("Nombre total de changements considerables sur Y : ");Serial.println(countY);
  Serial.print("Valeur moyenne sur X : ");Serial.println(meanX/6000);
  Serial.print("Valeur moyenne sur Y : ");Serial.println(meanY/6000);
  if( countX == 0 && countY ==0)
  {
    Serial.println("Vous n'avez pas bougé de façon considérable");  
  }
  else
  {
    if(countX > countY)
    {
     Serial.println("Il est plus probable que vous ayez bougé sur l'axe X");
    }
    else
    {
      Serial.println("Il est plus probable ue vous ayez bougé sur l'axe Y");
    }
  }
  Serial.println("--------------");
  //remise a zero des diiferences max : 
  max_diffX = 0;
  max_diffY = 0;
  //remise à zero des compteurs
  countX = 0;
  countY = 0;
  //remise à zero des valeurs moyennes
  meanX = 0;
  meanY = 0;
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print(" acceleration sur X: \t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print(" acceleration sur Y: \t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print(" acceleration sur Z: \t"); Serial.print(event.acceleration.z); Serial.print("\t");
  Serial.println("");
 // Serial.println("m/s^2 ");
  
//  /* Get the orientation of the sensor */
//  uint8_t o = mma.getOrientation();
//  
//  switch (o) {
//    case MMA8451_PL_PUF: 
//      Serial.println("Portrait Up Front");
//      break;
//    case MMA8451_PL_PUB: 
//      Serial.println("Portrait Up Back");
//      break;    
//    case MMA8451_PL_PDF: 
//      Serial.println("Portrait Down Front");
//      break;
//    case MMA8451_PL_PDB: 
//      Serial.println("Portrait Down Back");
//      break;
//    case MMA8451_PL_LRF: 
//      Serial.println("Landscape Right Front");
//      break;
//    case MMA8451_PL_LRB: 
//      Serial.println("Landscape Right Back");
//      break;
//    case MMA8451_PL_LLF: 
//      Serial.println("Landscape Left Front");
//      break;
//    case MMA8451_PL_LLB: 
//      Serial.println("Landscape Left Back");
//      break;
//    }

  
}
