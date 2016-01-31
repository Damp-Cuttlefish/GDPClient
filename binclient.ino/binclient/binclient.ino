#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <SD.h>

/* Function Declarations */
void wifi();
int rangefinder();
/* Variables sent to server */
byte mac[6]; 
byte maxdepth;
byte currentdepth;
byte batterystatus;
/* Variables recieved from server */
byte timevariable;
/* Wifi connect details */
const char* ssid     = "belkin.c23";
const char* password = "";
// int keyindex; // Key index for WEP encrpytion.

WiFiClient client;

void setup()
{
  Serial.begin(115200);
  wifi();

}

void loop() 
{

}

int rangefinder(void)
{
}

void wifi(void)
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  if(client.connect("yourwaifuaslut.xyz",80))
  { 
    Serial.println("connected to the damn server");
  }
}

