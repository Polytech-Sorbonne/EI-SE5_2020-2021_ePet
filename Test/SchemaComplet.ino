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

void detection_wifi(byte data_Sigfox[12]){
  
  Serial.println("scan start");
  String bssid;
  uint8_t rssi;
    
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    
    for (int i = 0; i < 2; ++i) {
      // Print BSSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.BSSIDstr(i));
      bssid = WiFi.BSSIDstr(i);
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      rssi = WiFi.RSSI(i);
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      
      //construction du message Sigfox
      //Code Sigfox frame (MAC (6B) + RSSI (2B))
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
      Serial.print((rssi >>  0) & 0xFF,HEX);
      Serial.print((rssi >>  8) & 0xFF,HEX);

      Serial.print("string version of rssi : ");
      Serial.println(String(rssi,HEX));
            
      data_Sigfox[6] =(rssi >>  0) & 0xFF;
            
      data_Sigfox[7] = (rssi >>  8) & 0xFF;
      Serial.println("");
            
    }
  }
}

void convFloatToHex(float x, byte res){
  int integer = x * 10;
  Serial.print("Integer : ");
  Serial.println(integer);
  res = integer & 0xFF;
  Serial.print("Hexa : ");
  Serial.println(res);
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
  
  // data_Sigfox[8] = event.acceleration.x
  convFloatToHex(event.acceleration.x, data_Sigfox[8]);
  // data_Sigfox[9] = event.acceleration.y
  convFloatToHex(event.acceleration.y, data_Sigfox[9]);
  // data_Sigfox[10] = event.acceleration.z
  //convFloatToHex(event.acceleration.z, data_Sigfox[10]);
}

// A tester
void convFloatToHexTemp(float x, byte res){
  int integer = x * 100;
  Serial.print("Integer : ");
  Serial.println(integer);
  res = integer & 0xFFFF;
  Serial.print("Hexa : ");
  Serial.println(res);
}
// Tester avec sigfox
void temperature(byte res){
  sensors_event_t humidity, temp;
  
  shtc3.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  int tmp = temp.temperature;
  delay(1000);
}

void loop()
{
    detection_wifi(data_Sigfox);
    accelerometre(mma, data_Sigfox);
    
    //Peut-être coder la fonction send_message pour envoyer des données
    sprintf(data_string, "AT$SF=%02x%02x\r\n",data_Sigfox[0],data_Sigfox[1]) ; // A compléter
    
    Serial2.print(data_string);
    Serial.println(sizeof(data_string));
    Serial.println("Envoi");
    Serial.println(data_string);
             
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
    
}