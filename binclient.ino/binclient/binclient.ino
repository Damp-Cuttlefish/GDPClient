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
char ssid[] = "ssidGoesHere";
char password[] = "passwordGoesHere"; // Used as password for WPA, or the Key for WEP.
// int keyindex; // Key index for WEP encrpytion.
/* On board LED */
int led = 13;


void setup()
{
WiFi.macAddress(mac); // Saves the devices mac address to mac, used as the UID.

}

void loop() 
{
  wifi();
}

int rangefinder(void)
{
}

int wifi()
{
  WiFi.begin(ssid,password);                // begin trying to connect to a WiFi network with WPA authentication.
  while(WiFi.status() != WL_CONNECTED);     // Wait while chip is connecting.
  while(WiFi.status() == WL_CONNECT_FAILED) // If WiFi connection fails, blink on board LED.
  {
   digitalWrite(led, HIGH);
   delay(1000);
   digitalWrite(led, LOW);
   delay(1000); 
  }
  while(WiFi.status() == WL_CONNECTED)      // If WiFi connection succeeds, Turn on board LED on.
  {
    digitalWrite(led, HIGH);
  }
}

