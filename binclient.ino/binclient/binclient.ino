#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h> //Does SSL/TLS

/* Function Declarations */
void wifi();
int rangefinder();
/* Variables sent to server */
byte mac[6];
byte maxdepth = 0;
byte currentdepth = 1;
byte batterystatus = 1;
/* Variables recieved from server */
byte timevariable;
/* Wifi connect details */
char* ssid     = "";
char* password = "";
// int keyindex; // Key index for WEP encrpytion.

WiFiClient client;

void setup()
{
  WiFi.macAddress(mac);
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
