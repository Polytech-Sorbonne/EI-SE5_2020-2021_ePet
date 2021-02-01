/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include "WiFi.h"

void setup()
{
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

void loop()
{
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < 2; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            // Serial.print(WiFi.SSID(i));
            Serial.print(WiFi.BSSIDstr(i));
            String bssid = WiFi.BSSIDstr(i);
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            uint8_t rssi = WiFi.RSSI(i);
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            //construction du message Sigfox
            //Code Sigfox frame (MAC (6B) + RSSI (5B))
            //for(int i = 0; i < 6; i++)
            // {
            //  data[i] = bssid[i + 6 * 0];
            // }
            //data[6] = (rssi[0] >>  0) & 0xFF;
            //data[7] = (rssi[0] >>  8) & 0xFF;
            //data[8] = (rssi[0] >> 16) & 0xFF;
            //data[9] = (rssi[0] >> 24) & 0xFF;
            byte data_Sigfox[11];
            for (int j = 0; j < 6; j++)
            {
              Serial.print("bssid[");
              Serial.print(j);
              Serial.print("]");
              String xval = getValue(bssid, ':', j);
              Serial.print(xval);
              Serial.println("");
              Serial.print("int version : ");
              Serial.print(xval.toInt());
              Serial.println("");
              data_Sigfox[j] = xval.toInt();
              Serial.print("hex version : ");
              Serial.print(data_Sigfox[j],HEX);
              Serial.println("");
            }
            Serial.print("RSSI:");
            Serial.print(rssi,HEX);
            data_Sigfox[6] = rssi;
            Serial.println("");
            delay(10);
        }
    }
    Serial.println("");

    // Wait a bit before scanning again
    delay(5000);
}
