/*
  Simple HTTP Client over GPRS
 
 This sketch establishs a GPRS connection and sends a HTTP request to a hardcoded URL
 
 Circuit:
 * Arduino with GSM shield attached
 * EMnify SIM card inserted into GSM shield
 
 */

// Include GSM library
#include <GSM.h>

// SIM PIN is disabled for EMnify SIMs by default
#define PIN ""

// APN configuration
#define GPRS_APN       "em" // EMnify APN Name
#define GPRS_USERNAME  ""   // leave blank for EMnify SIM
#define GPRS_PASSWORD  ""   // leave blank for EMnify SIM

// Host, Port and Path of URL to connect to
char host[] = "www.emnify.com";
int port = 80; // port 80  for HTTP
char path[] = "/arduino.html";

// Initialize libraries
GSMClient client;
GPRS gprs;
GSM gsmAccess; 

void setup()
{
  // initialize serial communications
  Serial.begin(9600);

  // GSM connection state
  boolean gsmConnected = false;

  Serial.println("Trying to attach to GPRS network...");

  while(!gsmConnected)
  {
    if (gsmAccess.begin(PIN) == GSM_READY)  
    {
      if(gprs.attachGPRS(GPRS_APN, GPRS_USERNAME, GPRS_PASSWORD) == GPRS_READY)
      {
        gsmConnected = true;
        Serial.println("Connected to GPRS network successfully");
      }
    else
      {
        Serial.println("GPRS not connected, retrying after 3 seconds...");
        delay(3000);
      }
    }
  }
}

int http_request()
{
  Serial.print("Connecting to ");
  Serial.print(host);
  Serial.print(":");
  Serial.println(port);

  // establish TCP connection
  if (client.connect(host, port))
  {
    Serial.println("Connected");    
    // Send HTTP request and request to close connection
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(host);
    client.println("Connection: close");
    client.println();
  } 
  else
  {
    // report connection failure
    Serial.println("connection failed");
    return(0);
  }

  // read and print incoming data or stop connection if we are disconnected

  for (;;)
  {
    if (client.available())
    {
      char c = client.read();
      Serial.print(c);
    }

    // if the server's disconnected, stop the client:
    if (!client.available() && !client.connected())
    {
      Serial.println();
      Serial.println("Disconnecting.");
      client.stop();

      return(1);  
    }
  }
}

void loop()
{
  int counter = 0;

  // repeat request every 60 seconds and count number of requests
  Serial.print("Trying to make http request number ");
  Serial.println(counter++);

  http_request();

  Serial.println("Sleeping 60 secs before next request");
  delay(60000);
}


