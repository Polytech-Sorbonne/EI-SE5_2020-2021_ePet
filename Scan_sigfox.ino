/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include "WiFi.h"

#define RX0 3 // PIN RX0 <=> D3
#define TX0 1 // PIN TX0 <=> D1

void setup()
{
      // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
    Serial2.begin(9600, SERIAL_8N1, RX0, TX0);
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");
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

void loop()
{
    Serial.println("scan start");
    String bssid;
    uint8_t rssi;
    byte data_Sigfox[11];
    
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
            for (int j = 0; j < 6; j++)
            {
//              Serial.print("bssid[");
//              Serial.print(j);
//              Serial.print("]");
              String xval = getValue(bssid, ':', j);
//              Serial.print(byte(xval[0]));
//              Serial.print(",");
//              Serial.print(byte(xval[1]));
//              Serial.println("");
//              Serial.print("int version : ");
//              Serial.print(convStringToInt(xval[0]) & 0xFF,HEX);
//              Serial.print(convStringToInt(xval[1]) & 0xFF,HEX);
              byte v1 = convStringToInt(xval[0]) & 0xFF;
              byte v2 = convStringToInt(xval[1]) & 0xFF;
//              Serial.println("");
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
            data_Sigfox[6] =(rssi >>  0) & 0xFF;
            data_Sigfox[7] = (rssi >>  8) & 0xFF,HEX;
            Serial.println("");


            //send to Sigfox
            //    while (Serial2.available()) {
            //Serial2.write(data_Sigfox, 11);
            String data_string;
            data_string = "AT$SF=" + String(data_Sigfox[0],HEX) + String(data_Sigfox[1],HEX) + "\r\n";
            Serial2.print(data_string);
            Serial.println("Envoi");
            Serial.print("AT$SF=" + String(data_Sigfox[0],HEX) + String(data_Sigfox[1],HEX) + "\r\n");
            

            
            delay(1000);
        }
    }
    Serial.println("");

    // Wait a bit before scanning again
    delay(5000);
}
