#include <SD.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
/* Function Declarations */
int wifi();
int rangefinder();
/* Variables sent to server */
byte mac[6]; 
byte maxdepth;
byte currentdepth;
byte batterystatus;
/* Variables recieved from server */
byte timevariable;
/* Wifi connect details */
// char ssid[] = "ssid";
// char password[] = "password"; // Used as password for WPA, or the Key for WEP.
// int keyindex; // Key index for WEP encrpytion.


void setup()
{
WiFi.macAddress(mac); // Saves the devices mac address to mac, used as the UID.


}

void loop() 
{
  // put your main code here, to run repeatedly:

}

int rangefinder(void)
{
}

int wifi()
{
}

