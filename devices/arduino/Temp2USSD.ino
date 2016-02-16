/*
 Temp2USSD
 
 Circuit:
 * GSM shield 
 * EMnify SIM card
 * Temp sensor 18B20/One Wire Bus
 * 4.7K Ohm Resistor
 
 */

#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// GSM shield connected to PINs 2 (RX) and  3 (TX)
SoftwareSerial gsmSerial(2, 3);

// DS18B20 data wire connected to PIN 4
#define ONE_WIRE_BUS 4

// Setup oneWire instance
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to Dallas Temperature library
DallasTemperature sensors(&oneWire);

// use LED on PIN 13 as control lamp
int led = 13;

void setup()
{
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("Temp Monitor initializing");

  pinMode(led, OUTPUT); 
  digitalWrite(led, HIGH);

  gsmSerial.begin(9600);
  sensors.begin();

  digitalWrite(led, LOW); 
}

char atCommand[100];
char atUssd[32]="AT+CUSD=1,\"";
char ussdPrefix[32]="*100*temp=";
char ussdSuffix[3]="#\"";
char tempStr[23];

void loop()
{
  int charsReceived;
  float temp;

  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); 
  Serial.println("Done");

  Serial.print("Temperature for Device 1 is: ");
  temp = sensors.getTempCByIndex(0);
  Serial.println(temp);

  // compose AT command with USSD string
  strcpy(atCommand, atUssd);
  strcat(atCommand, ussdPrefix);
  dtostrf(temp, 2, 2, tempStr);  
  strcat(atCommand, tempStr);
  strcat(atCommand, ussdSuffix);

  // send AT command
  Serial.println("Sending USSD Request");

  gsmSerial.println(atCommand);

  delay(2000);
  
  while (!(charsReceived = gsmSerial.available()))
  {
    digitalWrite(led, HIGH);  
    delay(100);
    digitalWrite(led, LOW);  
    delay(100);
  }

  for (;charsReceived > 0;charsReceived--)
  {
    Serial.write(gsmSerial.read());   
    delay(50);
  }

  Serial.println("");

  Serial.println("Done");

  Serial.println("Sleeping 120 secs");

  delay(120000);  
}

















