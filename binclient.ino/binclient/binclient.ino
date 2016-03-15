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
char ssid[100]     = "";
char password[100] = "";
int keyindex; // Key index for WEP encrpytion.
/* Variables */
#define mins5 10000                     // Milliseconds device waits 1) from setup button press to maxdepth reading or turn on 2) if button is pressed for < reset seconds
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
  
  EEPROM.begin(512);
  while(firstTime > 1 || firstTime < 0)  // Check whether first time setup has ever been ran before
    {
      firstTime = EEPROM.read(initilize);
      i++;
      if( i > 10)                        // If first time != 0 or 1, assume device needs first time setup
      {
        firstTime = 0;
        i = 0;
      }
      yield();
    }
  EEPROM.end();

  if(firstTime == 0)                    // First time setup
    {
      digitalWrite(led, HIGH);
      while(digitalRead(button) == HIGH)  // Wait for button to be pressed before 5 minute countdown
      {
        yield();
      }
      digitalWrite(led,LOW);
      delay(mins5);
      maxdepth = rangefinder(rangefinderwaitinsetup); // Take max depth reading
      EEPROM.begin(512);
      EEPROM.write(saveddepth,maxdepth);  // Save Max depth reading into non volitile memory
      EEPROM.write(initilize,1);          // Save the first time setup completion variable as complete
      EEPROM.end();
    }
  else if(firstTime == 1)                 // Normal startup, retrieve variables from memory
    {
      EEPROM.begin(512);
      maxdepth = EEPROM.read(saveddepth); // Get max depth read on last first time setup
      timevariable = EEPROM.read(savedtimevariable);  // Get time variable from last first time setup
      if(timevariable == 0)               // If saved variable was 0, change it to one or device will continuously run
        timevariable = 1;
      EEPROM.end();
      delay(mins5);
    }
  attachInterrupt(button, button_ISR, FALLING); // Initilise button interrupt
}

void loop()
{
  int i, j;
  currentdepth = rangefinder(waitbetweenlooprangefinding);  // Take curent dpeth reading
  wifi();                                 // Send data to server
  for(i = 0; i < seconds; i++)            // Wait for time variable hours
    for(j = 0; j < timevariable; j++)
      delay(1000);
}

int rangefinder(int timedelay)           // Takes depth reading and returns depth in centimeters
{
  int x1 = 0;
  int x2 = 0;
  byte ok = 0;
  int pulse;
  while( ok == 0 )              // Takes two readings 30 seconds apart and check to see if they are within 10cm of each other
  {
    digitalWrite(pwr, HIGH);      // Turn rangefinder on
    delay(250);                   //wait for 250ms for start up to complete

    digitalWrite(rxpin, HIGH);    // Command reading from rangefinder
    delayMicroseconds(20);
    digitalWrite(rxpin, LOW);
    delay(200);                   // Wait for confusing first reading to end

    digitalWrite(rxpin, HIGH);    // Command first reading from rangefinder
    delayMicroseconds(20);
    digitalWrite(rxpin, LOW);     
    x1 = pulseIn(pwpin, HIGH);    // Wait for first reading to come back
    x1 = x1 / 58;                 // Conver tot centimeters
    digitalWrite(pwr, LOW);       // Turn rangefinder off
    delay( 1000* timedelay );     // Wait timedelay seconds before second reading is taken
    digitalWrite(pwr, HIGH);      // Turn rangefinder on
    delay(250);                   // Wait for 250ms for start up to complete

    digitalWrite(rxpin, HIGH);    // Command reading from rangefinder
    delayMicroseconds(20);
    digitalWrite(rxpin, LOW);
    delay(200);                   // Wait for confusing first reading to end 

    digitalWrite(rxpin, HIGH);    // Command second reading from rangefinder
    delayMicroseconds(20);
    digitalWrite(rxpin, LOW);
    x2 = pulseIn(pwpin, HIGH);    // Wait until rangefinder sends it's reading back
    x2 = x2 / 58;                 // Convert to centimeters
    digitalWrite(pwr, LOW);       // Turn rangefidner off
    pulse = x1 - x2;              
    if( pulse > -10 && pulse < 10)  // Check to see if both readings are within 10 cm of each other
      {
        ok = 1;                   // If they are set ok to 1 so we can leave the rangefinding loop
      }                           // If they are not, take another set of readings
  }
  return x1;                      // Return good value
}

void wifi(void)
{
  byte batterystatus = 0;
  byte mac[6];
  WiFi.macAddress(mac);           // Get devices MAC address, this serves as its UID
  if(digitalRead(battery) == HIGH)  // Check if the battery is low
    batterystatus = 1;

  String data = String("UID=") ;  // Format all the data that is sent to the server
  data = String(data + mac[0] + mac[1] + mac[2] + mac[3] + mac[4] + mac[5]);
  data = String(data + "&BinLevelCurrent=");
  data = String(data + currentdepth);
  data = String(data + "&BinLevelMax=");
  data = String(data + maxdepth);
  data = String(data + "&Status=");
  data = String(data + batterystatus);


  WiFi.begin(ssid, password);   // initilise the connection to the WiFi access point

  while (WiFi.status() != WL_CONNECTED) // Wait until we are connected to access point
  {
    yield();
  }
  if (client.connect("binformant.tk", 80))  // Connect to binformant.tk 
  {
    client.println("POST /client.php HTTP/1.1");  // Send correct http headers
    client.println("Host: binformant.tk");
    client.println("User-Agent: Arduino");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);                   // Send data
    client.stop();                          // Disconnect
  }
}

void button_ISR(void) // Button press ISR
{
  long count = 0;
  long time1;
  long time2;
  int i;
  time1 = millis();   // Take time reading straight after function is called
  while (digitalRead(button) == LOW)  // Wait untill button is let go of
    ESP.wdtFeed();    // Stops WDT setting off
  time2 = millis();   // Take second time reading
  count = time2 - time1;  // Find out how many milliseconds button has been pressed for
  if( count >= (resetseconds*1000)) // Device reset routine if button pressed for longer than reset seconds
  {
    EEPROM.begin(512);
    EEPROM.write(initilize, 0);     // Tell device it has never had a first time setup
    EEPROM.write(savedtimevariable, 0); // Override previous time variable
    EEPROM.end();
    digitalWrite(16, LOW);          // Reset device using reset line
  }
  else  // Otherwise button press is for program pause
  {
    time1 = 0;
    time2 = 0;
    time1 = millis(); // Take time reading 
    while(count < mins5)  // Wait until count has reached mins5 variable in milliseconds
    {
      time2 = millis();   // take time
      count = time2 - time1;  // calculate how many milliseconds it has been
      ESP.wdtFeed();  // Stop WDT setting off
    }
  }
}
