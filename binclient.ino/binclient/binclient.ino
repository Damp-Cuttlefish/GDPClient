#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h> //Does SSL/TLS
WiFiClient client;
/* Function Declarations */
void wifi();
int rangefinder();
int sd();
void button_ISR();
/* Variables sent to server */
byte maxdepth = -1;
byte currentdepth = 0;
/* Random globals */
byte timevariable = 6;
int x1 = 0;
int x2 = 0;
byte ok = 0;
/* Wifi connect details */
char* ssid     = "belkin.c23";
char* password = "";
int keyindex; // Key index for WEP encrpytion.
/* Variables */
#define mins5 300000        // 5 Minute delay

/* Pins definitions */
#define pwpin 13            // Return signal from rangefinder
#define rxpin 12            // Rangefind command pin
#define pwr 14              // Rangefinder power pin
#define battery 15           // Battery level circuit pin
#define button  2           // Button pin
#define led 4               // LED output pin


/* EEPROM variable addresses */
#define saveddepth 100
#define savedtimevariable 101
#define initilize 102

void setup()
{
  byte i = 0;
  int j = 0;/* These little buddies are used for the rangefinding check! */

  byte firstTime = -1;
  Serial.begin(115200);
  /* Pin IO */
  pinMode(pwr, OUTPUT);
  pinMode(pwpin, OUTPUT);
  pinMode(rxpin, INPUT);
  pinMode(battery, INPUT);
  pinMode(button, INPUT);
  pinMode(16, OUTPUT);
  Serial.print("pins done");
  digitalWrite(16, HIGH);
  if(digitalRead(battery) == HIGH)          // If battery is low on startup blink LED forever
  {
    while(1)    
    {
      digitalWrite(led, HIGH);
      delay(500);
      digitalWrite(led, LOW);
      delay(500);
    }
  }
  Serial.println("batterycheck passed");
  /* SD card stuff here */
  Serial.println("initilise begin");
  EEPROM.begin(512);
  while(firstTime > 1 || firstTime < 0)  // Check whether first time setup has ever been ran before
    {
      firstTime = EEPROM.read(initilize);
      i++;
      if( i > 10)                         // If the memory is something other than 1 or 0 this will assume first time setup has never been run, or firstTime would be 1
      {
        firstTime = 0;
        i = 0;
      }
      yield();
      Serial.println("initilize looped");
    }
  EEPROM.end();
  Serial.println("initilise passed");
  if(firstTime == 0)
    {
      Serial.println("firsttime = 0");
      digitalWrite(led, HIGH);
      while(digitalRead(button)==0)// Wait for button to be pressed before 5 minute countdown
      {
        Serial.println("waiting for button press");
        yield();
      }
      digitalWrite(led,LOW);
      delay(500);
      maxdepth = rangefinder();
      Serial.println(maxdepth);
      EEPROM.begin(512);
      EEPROM.write(saveddepth,maxdepth);  // Save Max depth reading into non volitile memory
      byte doesthiswork = 1;
      EEPROM.write(initilize,doesthiswork);         // Save the first time setup completion variable as complete
      Serial.print(" eeprom initilize saved number = ");
      Serial.println(EEPROM.read(initilize));
      Serial.print(" eeprom maxdepth saved number = ");
      Serial.println(EEPROM.read(saveddepth));
      EEPROM.end();
      Serial.println("first time = 0 done");
    }
  else if(firstTime == 1)
    {
      Serial.println("First time = 1");
      EEPROM.begin(512);
      maxdepth = EEPROM.read(saveddepth);
      timevariable = EEPROM.read(savedtimevariable);
      if(timevariable == 0)
        timevariable = 1;
      EEPROM.end();
      Serial.println("first time = 1 done");
    }
  attachInterrupt(button, button_ISR, RISING);
}

void loop()
{
  byte i;
  int j;
  while( ok == 0 )              // Takes two readings 30 secodns apart and check to see if they are within 10cm of each other! pretty neat right
  {
    x1 = rangefinder();
    Serial.print("30s start x1 = ");
    Serial.println(x1);
    delay(30000);               // Wait 30 seconds
    x2 = rangefinder();
    Serial.print("30s end x2 = ");
    Serial.println(x2);
    currentdepth = x1 - x2;
    if( currentdepth > -10 && currentdepth < 10)
      {
        ok = 1;
        currentdepth = x1;
      }
  }
  ok = 0;
  wifi();

  delay(1000 * 3600 * timevariable);
  Serial.println(currentdepth);
  Serial.println("loop looped");
}

int rangefinder(void)         // Returns range in cm
{
  int pulse = 0;

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
  //CONNECTION_TYPE;SSID;PASSWORD;
  byte serialArray[64] = {0};
  byte i;
  if(Serial.available() > 0)
  {
    Serial.readBytesUntil(';',serialArray, 32);
    for(i = 0; i < 64; i++)
      Serial.print(serialArray[i]);
  }
  
  
}

void wifi(void)
{
  byte batterystatus = 0;
  byte mac[6];
  WiFi.macAddress(mac);
  if(digitalRead(battery) == 1)
    batterystatus = 1;

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

void button_ISR(void)
{
  int count = 0;
  while (button == 1)
  {
    count+1;
    delay(1);
  }
  if( count >= 5000)
  {
    EEPROM.begin(512);
    EEPROM.write(initilize, 0);
    EEPROM.end();
    setup();
    digitalWrite(led,HIGH);   // CHECK THESE NEXT 3 LINES TO SEE IF THEY COMPILE RIGHT
    delay(500);
    digitalWrite(led,LOW);
  }
  else delay(mins5);
}
