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
byte maxdepth = 0;
byte currentdepth = 1;
byte batterystatus = 1;
/* Variables recieved from server */
byte timevariable;
/* Wifi connect details */
const char* ssid     = "belkin.c23";
const char* password = "";
// int keyindex; // Key index for WEP encrpytion.
String memes = "Id just like to interject for a moment. What youre referring to as Linux, is in fact, GNU/Linux, or as Ive recently taken to calling it, GNU plus Linux. Linux is not an operating system unto itself, but rather another free component of a fully functioning GNU system made useful by the GNU corelibs, shell utilities and vital system components comprising a full OS as defined by POSIX. Many computer users run a modified version of the GNU system every day, without realizing it. Through a peculiar turn of events, the version of GNU which is widely used today is often called Linux, and many of its users are not aware that it is basically the GNU system, developed by the GNU Project. There really is a Linux, and these people are using it, but it is just a part of the system they use. Linux is the kernel: the program in the system that allocates the machine’s resources to the other programs that you run. The kernel is an essential part of an operating system, but useless by itself; it can only function in the context of a complete operating system. Linux is normally used in combination with the GNU operating system: the whole system is basically GNU with Linux added, or GNU/Linux. All the so-called Linux distributions are really distributions of GNU/Linux.";
WiFiClient client;

void setup()
{
  Serial.begin(115200);
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

  if (client.connect("yourwaifuaslut.xyz", 80))
  {
    Serial.println("connected to the damn server");
    client.println("POST /public/GDPServer/ctest.php HTTP/1.1");
    client.println("Host: yourwaifuaslut.xyz");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: ");
    client.print(memes.length());
    client.println();
    client.println(memes);


    Serial.println("Done!");
  }

}




