// Librairies Accéléromètre
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

// Librairies Réseaux Wifi
#include "WiFi.h"
#include <HardwareSerial.h>

//Librairies Température
#include "Adafruit_SHTC3.h"

#define RX0 16 // PIN RX0 <=> D3
#define TX0 17 // PIN TX0 <=> D1

Adafruit_MMA8451 mma = Adafruit_MMA8451();

// Message SigFox
byte data_Sigfox[12];
byte data_Sigfox2[12];
char data_string[12];
String status = "";
char output;


Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
#define I2C_SDA 21
#define I2C_SCL 22

TwoWire I2CBME = TwoWire(0);

void setup()
{
    // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
    Serial2.begin(9600);
    Serial.begin(115200);
    pinMode(RX0, INPUT);
    pinMode(TX0, OUTPUT);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");

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

    Serial.println("SHTC3 test");
  if (! shtc3.begin()) {
    Serial.println("Couldn't find SHTC3");
    while (1) delay(1);
  }
  Serial.println("Found SHTC3 sensor");
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int convStringToInt(char s){
  if(int(s) >= '0' && int(s) <= '9'){
//    Serial.println("Conversion :");
//    Serial.println(int(s) - '0');
    return int(s) - '0'; 
  }
  else if(int(s) >= 'A' && int(s) <= 'F'){
//    Serial.println("Conversion :");
//    Serial.println(int(s) - 55);
    return int(s) - 55 ;
  }
  return 0;
}

void detection_wifi(byte data_Sigfox[12], byte data_Sigfox2[12]){
  
  Serial.println("scan start");
  String bssid;
  int8_t rssi;
    
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
      
    // Print BSSID and RSSI for each network found      
    //construction du message Sigfox pour l'adresse mac la  + puissante
    
    //Code Sigfox frame (MAC (6B) + RSSI (2B))
    bssid = WiFi.BSSIDstr(0);
    rssi = WiFi.RSSI(0);
    for (int j = 0; j < 6; j++){
      String xval = getValue(bssid, ':', j);
      byte v1 = convStringToInt(xval[0]) & 0xFF;
      byte v2 = convStringToInt(xval[1]) & 0xFF;
      data_Sigfox[j] = (v1 << 4 ) + v2 ;
      Serial.print("hex version : ");
      Serial.print(data_Sigfox[j],HEX);
      Serial.println("");
    }
      
    Serial.print("RSSI:");
    Serial.print(rssi);
    Serial.println("");
    Serial.println((rssi >>  0) & 0xFF,HEX);
    Serial.println((rssi >>  8) & 0xFF,HEX);
    Serial.print("string version of rssi : ");
    Serial.println(String(rssi,HEX));
            
    data_Sigfox[6] =(rssi >>  0) & 0xFF;
    Serial.println("");

    //construction du message Sigfox pour l'adresse mac la 2eme + puissante
    //construction du message Sigfox pour l'adresse mac la  + puissante
    
    //Code Sigfox frame (MAC (6B) + RSSI (2B))
    bssid = WiFi.BSSIDstr(1);
    rssi = WiFi.RSSI(1);
    for (int j = 0; j < 6; j++){
      String xval = getValue(bssid, ':', j);
      byte v1 = convStringToInt(xval[0]) & 0xFF;
      byte v2 = convStringToInt(xval[1]) & 0xFF;
      data_Sigfox2[j] = (v1 << 4 ) + v2 ;
      Serial.print("hex version : ");
      Serial.print(data_Sigfox2[j],HEX);
      Serial.println("");
    }
      
    Serial.print("RSSI:");
    Serial.print(rssi);
    Serial.println("");
    Serial.println((rssi >>  0) & 0xFF,HEX);
    Serial.println((rssi >>  8) & 0xFF,HEX);
    Serial.print("string version of rssi : ");
    Serial.println(String(rssi,HEX));
            
    data_Sigfox2[6] =(rssi >>  0) & 0xFF;

    Serial.println("");
  }
}

byte convFloatToHex(float x){
  int integer = x * 10;
  Serial.print("Integer : ");
  Serial.println(integer);
  byte res = integer & 0xFF;
  Serial.print("Hexa : ");
  Serial.println(res);
  return res ;
}

void accelerometre(Adafruit_MMA8451 mma, byte data_Sigfox[12]){
 
  /* Get a new sensor event */ 
  sensors_event_t event; 
  mma.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: \t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print("Y: \t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print("Z: \t"); Serial.print(event.acceleration.z); Serial.print("\t");
  Serial.println("m/s^2 ");

  //Ajouter le formatage des données pour Sigfox
  
  // data_Sigfox[7] = event.acceleration.x
//  byte res;
//  data_Sigfox[7] = convFloatToHex(event.acceleration.x);
//  Serial.print("data Sigfox 7 : ");
//  Serial.println(data_Sigfox[7], HEX);
//  
//  // data_Sigfox[8] = event.acceleration.y
//  data_Sigfox[8] = convFloatToHex(event.acceleration.y);
//  Serial.print("data Sigfox 8 : ");
//  Serial.println(data_Sigfox[8], HEX);
//
//  // data_Sigfox[9] = event.acceleration.y
//  data_Sigfox[9] = convFloatToHex(event.acceleration.z);
//  Serial.print("data Sigfox 9 : ");
//  Serial.println(data_Sigfox[9], HEX);
}

// A tester
int convFloatToInt(float x){
  int integer = x * 100;
  Serial.print("Integer : ");
  Serial.println(integer);
  return integer ;
}

// Tester avec sigfox
int temperature(){
  sensors_event_t humidity, temp;
  
  shtc3.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  //Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  float tmp = temp.temperature;
  int res = convFloatToInt(tmp);
  delay(1000);
  return res ;
}


void send_message(byte data_Sigfox[12] , int temp){

   char* c = (char*) malloc( 12 * sizeof(char)) ;
   snprintf(c, 12 , "AT$SF=%02x%02x\r\n",data_Sigfox[0],data_Sigfox[1]) ;

//  String c = String("AT$SF=");
//  for(int i = 0; i < 7; i++){
//    if(data_Sigfox[i] < 16){
//      c.concat(String('0'));  
//    }
//    c.concat(String(data_Sigfox[i], HEX));
//  }
//  if (temp < 4096) {
//   c.concat(String('0')); 
//  }
//  c.concat(String(temp, HEX));
  Serial2.print(c);
  //Serial.println(sizeof(c));
  Serial.println("Envoi");
  Serial.println(c);
  delay(1000);    
  while (!Serial2.available()){
    Serial.println("Waiting for response");
    delay(1000);
  }
  while (Serial2.available()){
    output = (char)Serial2.read();
    status += output;
    delay(10);
  }
  Serial.println();
  Serial.print("Status \t");
  Serial.println(status);
  delay(1000);
    
  Serial.println("");
  // Wait a bit before scanning again
  delay(5000);  
  free(c);
}

void loop()
{ 
    int temp;
     //1er message 
    detection_wifi(data_Sigfox,data_Sigfox2);
    accelerometre(mma, data_Sigfox);
    temp = temperature();
    Serial.println("1er message");
    send_message(data_Sigfox, temp);
    //2eme message
    accelerometre(mma, data_Sigfox2);
    temp = temperature();
    send_message(data_Sigfox2, temp);
    
}
