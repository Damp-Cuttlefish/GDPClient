#include <SPI.h>
#include <SD.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h> //Does SSL/TLS

/* Function Declarations */
void wifi();
int rangefinder();
/* Variables sent to server */
int maxdepth;
int currentdepth;
byte batterystatus;
/* Variables recieved from server */
byte timevariable;
/* Wifi connect details */
char* ssid     = "";
char* password = "";
int keyindex; // Key index for WEP encrpytion.
/* Rangefinding Pins */
#define pwpin 13
#define rxpin 12
#define pwr 14

WiFiClient client;

void setup()
{
  Serial.begin(115200);
  maxdepth = rangefinder();
  Serial.println();
  Serial.println(maxdepth);
}

void loop()
{

}

int rangefinder(void)
{
  int pulse = 0;

  pinMode(pwr, OUTPUT);
  pinMode(pwpin, OUTPUT);
  pinMode(rxpin, INPUT);

  digitalWrite(pwr, HIGH);      //set pin 6 high
  delay(250);                   //wait for 250ms for start up to complete

  digitalWrite(rxpin, HIGH);    //set pin 4 high for 1ms
  delayMicroseconds(20);
  digitalWrite(rxpin, LOW);
  delay(200);

  digitalWrite(rxpin, HIGH);    //set pin 4 high for 1ms
  delayMicroseconds(20);
  digitalWrite(rxpin, LOW);
  pulse = pulseIn(pwpin, HIGH); //listen for return signal
  pulse = pulse / 58;
  digitalWrite(pwr, LOW);       //set pin 6 low
  return pulse;
}

int sd(void)
{
  if (digitalRead(/*the pin card detect is on*/1) == 1)
  {
    Serial.println("SD card present");
    SD.begin(/*insert pin the SD CS is connected to*/1);

  }
  else
    Serial.println("SD card not detected");
}

void wifi(void)
{
  byte mac[6];
  WiFi.macAddress(mac);

  String data = String("ID=") ;
  data = String(data + mac[0] + mac[1] + mac[2] + mac[3] + mac[4] + mac[5]);
  data = String(data + "&level=");
  data = String(data + currentdepth);
  data = String(data + "&levelmax=");
  data = String(data + maxdepth);
  data = String(data + "&battlevel=");
  data = String(data + batterystatus);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  if (client.connect("yourwaifuaslut.xyz", 80))
  {
    client.println("POST /public/GDPServer/ctest.php HTTP/1.1");
    client.println("Host: yourwaifuaslut.xyz");
    client.println("User-Agent: Arduino");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);
    client.stop();
  }
}
