#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h> //Does SSL/TLS
WiFiClient client;

/* Function Declarations */
void wifi();
int rangefinder(int);
int sd();
void button_ISR();
/* Variables sent to server */
byte maxdepth = -1;
byte currentdepth = 0;
/* Random globals */
byte timevariable = 6;
/* Wifi connect details */
const char* ssid     = "";
const char* password = "";
int keyindex; // Key index for WEP encrpytion.
/* Variables */
#define mins5 10000                     // Milliseconds device waits 1) from setup button press to maxdepth reading 2) if button is pressed for < reset seconds
#define waitbetweenlooprangefinding 5   // seconds between the two rangefinder measurments in loop()
#define rangefinderwaitinsetup 2        // Seconds between rangefinder measurements for maxdepth
#define seconds 2                       // Seconds * 1second * timevariable
#define resetseconds 5                  // Seconds you have to hold button down for system reset to happen

/* Pins definitions */
#define pwpin 13                        // Return signal from rangefinder
#define rxpin 12                        // Rangefind command pin
#define pwr 14                          // Rangefinder power pin
#define battery 15                      // Battery level circuit pin
#define button  2                       // Button pin
#define led 4                           // LED output pin


/* EEPROM variable addresses */
#define saveddepth 100
#define savedtimevariable 101
#define initilize 102

void setup()
{
  Serial.begin(115200);
  byte i = 0;
  int j = 0;
  byte firstTime = -1;
  /* Pin IO */
  pinMode(pwr, OUTPUT);
  pinMode(pwpin, OUTPUT);
  pinMode(rxpin, INPUT);
  pinMode(battery, INPUT);
  pinMode(button, INPUT);
  pinMode(16, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(16, HIGH);
  
  if(digitalRead(battery) == LOW)         // 2 Second pulse for good battery check on startup
  {
    digitalWrite(led, HIGH);
    delay(2000);
    digitalWrite(led, LOW);
  }
  else if(digitalRead(battery) == HIGH)   // 10 pulses for bad battery check on startup
  {
    for(i = 0; i < 10; i++)
    {
      digitalWrite(led, HIGH);
      delay(500);
      digitalWrite(led, LOW);
      delay(500);
    }
  }
  /* SD card stuff here */
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
    }
  EEPROM.end();

  if(firstTime == 0)
    {
      digitalWrite(led, HIGH);
      while(digitalRead(button) == HIGH)// Wait for button to be pressed before 5 minute countdown
      {
        yield();
      }
      digitalWrite(led,LOW);
      delay(mins5);
      maxdepth = rangefinder(rangefinderwaitinsetup);
      EEPROM.begin(512);
      EEPROM.write(saveddepth,maxdepth);  // Save Max depth reading into non volitile memory
      EEPROM.write(initilize,1);         // Save the first time setup completion variable as complete
      EEPROM.end();
    }
  else if(firstTime == 1)
    {
      EEPROM.begin(512);
      maxdepth = EEPROM.read(saveddepth);
      timevariable = EEPROM.read(savedtimevariable);
      if(timevariable == 0)
        timevariable = 1;
      EEPROM.end();
    }
  attachInterrupt(button, button_ISR, FALLING);
}

void loop()
{
  int i, j;
  currentdepth = rangefinder(waitbetweenlooprangefinding);
  wifi();
  for(i = 0; i < seconds; i++)                     
    for(j = 0; j < timevariable; j++)
      delay(1000);
}

int rangefinder(int timedelay)           // Returns range in cm
{
  int x1 = 0;
  int x2 = 0;
  byte ok = 0;
  int pulse;
  while( ok == 0 )              // Takes two readings 30 secodns apart and check to see if they are within 10cm of each other! pretty neat right
  {
    digitalWrite(pwr, HIGH);      //set pin 6 high
    delay(250);                   //wait for 250ms for start up to complete

    digitalWrite(rxpin, HIGH);    //set pin 4 high for 1ms
    delayMicroseconds(20);
    digitalWrite(rxpin, LOW);
    delay(200);

    digitalWrite(rxpin, HIGH);    //set pin 4 high for 1ms
    delayMicroseconds(20);
    digitalWrite(rxpin, LOW);
    x1 = pulseIn(pwpin, HIGH); //listen for return signal
    x1 = x1 / 58;
    digitalWrite(pwr, LOW);       //set pin 6 low
    delay( 1000* timedelay );               // Wait timedelay seconds
    digitalWrite(pwr, HIGH);      //set pin 6 high
    delay(250);                   //wait for 250ms for start up to complete

    digitalWrite(rxpin, HIGH);    //set pin 4 high for 1ms
    delayMicroseconds(20);
    digitalWrite(rxpin, LOW);
    delay(200);

    digitalWrite(rxpin, HIGH);    //set pin 4 high for 1ms
    delayMicroseconds(20);
    digitalWrite(rxpin, LOW);
    x2 = pulseIn(pwpin, HIGH); //listen for return signal
    x2 = x2 / 58;
    digitalWrite(pwr, LOW);       //set pin 6 low
    pulse = x1 - x2;
    if( pulse > -10 && pulse < 10)
      {
        ok = 1;
      }
  }
  return x1;
}

void wifi(void)
{
  byte batterystatus = 0;
  byte mac[6];
  WiFi.macAddress(mac);
  if(digitalRead(battery) == HIGH)
    batterystatus = 1;

  String data = String("UID=") ;
  data = String(data + mac[0] + mac[1] + mac[2] + mac[3] + mac[4] + mac[5]);
  data = String(data + "&BinLevelCurrent=");
  data = String(data + currentdepth);
  data = String(data + "&BinLevelMax=");
  data = String(data + maxdepth);
  data = String(data + "&Status=");
  data = String(data + batterystatus);


  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    yield();
  }
  if (client.connect("binformant.tk", 80))
  {
    client.println("POST /client.php HTTP/1.1");
    client.println("Host: binformant.tk");
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
  long count = 0;
  long time1;
  long time2;
  int i;
  time1 = millis();
  while (digitalRead(button) == LOW)
    ESP.wdtFeed();
  time2 = millis();
  count = time2 - time1;
  if( count >= (resetseconds*1000))
  {
    EEPROM.begin(512);
    EEPROM.write(initilize, 0);
    EEPROM.write(savedtimevariable, 0);
    EEPROM.end();
    digitalWrite(16, LOW);
  }
  else
  {
    time1 = 0;
    time2 = 0;
    time1 = millis();
    while(count < mins5)              // This sets the WDT off still
    {
      time2 = millis();
      count = time2 - time1;
      ESP.wdtFeed();
    }
  }
}
