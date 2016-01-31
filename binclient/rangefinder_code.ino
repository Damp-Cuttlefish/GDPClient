

void rangefinding(void)
{
  const int pwpin = 13;
  const int rxpin = 12;
  const int power = 14;
  int pulse = 0;

  pinMode(power, OUTPUT);
  pinMode(pwpin, OUTPUT);
  pinMode(rxpin, INPUT);
  digitalWrite(power, HIGH);    //set pin 6 high
  delay(250)  ;                 //wait for 250ms for start up to complete
  digitalWrite(rxpin, HIGH);    //set pin 4 high for 1ms
  delay(1);
  digitalWrite(rxpin, LOW);
  pulse = digitalRead(rxpin);    //listen for return signal
                                //possible delay required
                                //any manual returning of a value to be done?
  digitalWrite(power, LOW);     //set pin 6 low
}

