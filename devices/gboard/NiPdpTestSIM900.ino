/*
 
 Sketch to demonstrate
 Network Initiated PDP Context
 
 Hardware: ITEAD GBoard with SIMcom SIM800 Module + EMnify SIM Card
 
 PIN14 (A0) = red LED
 PIN15 (A1) = green LED
 
 (C) 2016 EMnify GmbH
 
 */

#include <SoftwareSerial.h>

#define MAX_RX_WAIT_CYCLES 400
#define RX_BUFFER_LENGTH 512

SoftwareSerial gsmSerial(2, 3); // RX, TX

int gsmPower = 6;
int gsmReset = 7;
int redLed = 14;
int greenLed= 15;

void setup()  
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }

  Serial.println("Starting ....");

  pinMode(redLed, OUTPUT); 
  digitalWrite(redLed, HIGH); 

  pinMode(greenLed, OUTPUT); 
  digitalWrite(greenLed, LOW); 

  gsmSerial.begin(9600); 

  Serial.println("Switching on GSM Module...");
  pinMode(gsmReset, OUTPUT);  
  digitalWrite(gsmReset, LOW); 

  do {
    pinMode(gsmPower, OUTPUT);       
    digitalWrite(gsmPower, LOW); 
    delay(1000);
    digitalWrite(gsmPower, HIGH); 
    delay(5000);
  }
  while (!atCommandWithTimeout("AT", 10));

  Serial.println("Modem switched on.");

  atCommand("AT");
  atCommand("ATE0");
  atCommand("AT+CGMM");
  atCommand("AT+CREG=0");
  atCommand("AT+CGREG=1");
  atCommand("AT+CRC=1");
  atCommand("AT+CGCLASS=\"B\"");
  atCommand("AT+CGEREP=1");
  atCommand("AT+CMEE=1");
  atCommand("AT+CIMI");
  atCommand("AT+COPS=0"); 
}

int niPdPRequest = false;

int atCommand(char *atCommand)
{
  return(atCommandWithTimeout(atCommand, MAX_RX_WAIT_CYCLES));
}

int atCommandWithTimeout(char *atCommand, int maxRxWaitCycles)
{
  int led;
  int bytesReceivedCounter = 0;
  int timeoutCounter = 0;
  char receiveBufferString[RX_BUFFER_LENGTH];

  if (strlen(atCommand))
  {
    gsmSerial.println(atCommand);

    Serial.print("TX: '");
    Serial.print(atCommand);  
    Serial.println("'");
  }

  if (niPdPRequest)
  {
    led = greenLed;
  }
  else
  {
    led =redLed;
  }

  while (!gsmSerial.available())
  {    
    digitalWrite(led, LOW); 
    delay(250);
    digitalWrite(led, HIGH); 
    delay(250);

    timeoutCounter++;

    if (timeoutCounter > maxRxWaitCycles)
    {    
      return false;
    }
  }

  Serial.print("RX: '");

  while (gsmSerial.available() && bytesReceivedCounter < RX_BUFFER_LENGTH)
  {
    char c;
    c=gsmSerial.read();  
    receiveBufferString[bytesReceivedCounter]=c;
    bytesReceivedCounter++;
  }

  receiveBufferString[bytesReceivedCounter]='\0';

  Serial.println(receiveBufferString);
  Serial.print("' (");

  if (strstr(receiveBufferString, "RING"))
  {
    niPdPRequest = true;
  }

  Serial.print(bytesReceivedCounter, DEC);
  Serial.println(" Bytes)");

  return true;
}

void httpGet(char *url)
{
  char httpPara[128];

  // configure bearer as GPRS
  atCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  atCommand("AT+SAPBR=3,1,\"APN\",\"em\"");
  // open bearer
  atCommand("AT+SAPBR=1,1");
  delay(2500);
  // query bearer
  atCommand("AT+SAPBR=2,1");

  // perform HTTP request
  atCommand("AT+HTTPINIT");
  atCommand("AT+HTTPPARA=\"CID\",1");
  strcpy(httpPara, "AT+HTTPPARA=\"URL\",\"");
  strcat(httpPara, url);
  strcat(httpPara, "\"");
  atCommand(httpPara);
  atCommand("AT+HTTPACTION=0");
  delay(10000);
  atCommand("");

  // close bearer/shutdown PDP Context
  atCommand("AT+SAPBR=0,1");
}

void loop() // run over and over
{  
  atCommand("AT+CREG?");
  atCommand("AT+CGREG?");
  atCommand("AT+CGATT?");
  atCommand("AT+COPS?");
  atCommand("AT+CSQ");

  atCommand("");

  if (niPdPRequest)
  {
    digitalWrite(redLed, LOW); 
    digitalWrite(greenLed, HIGH); 

    Serial.println("*** PDP CONTEXT REQUESTED BY NETWORK ***");

    httpGet("http://www.emnify.com/");

    niPdPRequest=false;

    digitalWrite(redLed, HIGH); 
    digitalWrite(greenLed, LOW); 
  }
}







































