/*
  WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>
#include "webpage.h"
#include "info.h"
#include "libESPdom.h"

#define _POST_REC
//#define _GET_REC

#define RELAY1_PIN (14)
#define RELAY2_PIN (12)
#define LED4_PIN (5)
#define LED5_PIN (13)

const char* ssid     = "ssid";
const char* password = "password";

bool isSendStateInfo;
bool isSendStateRelay1;
bool isSendStateRelay2;
WiFiServer server(80);
libESPdom espdom;

void exOperations(WiFiClient *client)
{
  if(isSendStateInfo)
  {
    isSendStateInfo=false;
    espdom.sendHttp(client,(char*)info);
    Serial.println("info sent");                    // print a message out the serial port
  }
  else if(isSendStateRelay1)
  {
    isSendStateRelay1=false;
    sendRelay1State(client);
    Serial.println("relay1 state sent");           // print a message out the serial port
  }
  else if(isSendStateRelay2)
  {
    isSendStateRelay2=false;
    sendRelay2State(client);
    Serial.println("relay2 state sent");           // print a message out the serial port
  }
  else
  {
    espdom.sendHttp(client,(char*)webpage);
    Serial.println("HTML sent");                    // print a message out the serial port
  }
}

void parseCurrentLine(String *currentLine)
{
  if(currentLine->endsWith("ORD&info"))           // /ORD&info send json info string.
  {
    isSendStateInfo=true;
    Serial.println("read info");                      // print a message out the serial port
  }
  else if(currentLine->endsWith("ORD&relay1"))         // /ORD&relay1 read relay1 status.
  {
    isSendStateRelay1=true;
    Serial.println("read relay1 state");              // print a message out the serial port
  }
  else if(currentLine->endsWith("OWR&relay1=H"))       // /OWR&relay1=H turns the relay1 on.
  {
    digitalWrite(RELAY1_PIN,HIGH);
    isSendStateRelay1=true;
    Serial.println("write relay1 state H");           // print a message out the serial port
  }
  else if(currentLine->endsWith("OWR&relay1=L"))       // /OWR&relay1=L turns the relay1 off.
  {
    digitalWrite(RELAY1_PIN,LOW);
    isSendStateRelay1=true;
    Serial.println("write relay1 state L");           // print a message out the serial port
  }
  else if(currentLine->endsWith("ORD&relay2"))         // /ORD&relay2 read relay2 status.
  {
    isSendStateRelay2=true;
    Serial.println("read relay2 state");              // print a message out the serial port
  }
  else if(currentLine->endsWith("OWR&relay2=H"))       // /OWR&relay2=H turns the relay2 on.
  {
    digitalWrite(RELAY2_PIN,HIGH);
    isSendStateRelay2=true;
    Serial.println("write relay2 state H");           // print a message out the serial port
  }
  else if(currentLine->endsWith("OWR&relay2=L"))       // /OWR&relay2=L turns the relay2 off.
  {
    digitalWrite(RELAY2_PIN,LOW);
    isSendStateRelay2=true;
    Serial.println("write relay2 state L");           // print a message out the serial port
  }
}

void sendRelay1State(WiFiClient *client)
{
  if(digitalRead(RELAY1_PIN))
    espdom.sendValueVObject(client,"relay1","H");
  else
    espdom.sendValueVObject(client,"relay1","L");

  Serial.println("relay1 state sent");           // print a message out the serial port
}

void sendRelay2State(WiFiClient *client)
{
  if(digitalRead(RELAY2_PIN))
    espdom.sendValueVObject(client,"relay2","H");
  else
    espdom.sendValueVObject(client,"relay2","L");
    
  Serial.println("relay2 state sent");           // print a message out the serial port
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED4_PIN, OUTPUT);      // set the LED4 pin mode
    pinMode(LED5_PIN, OUTPUT);      // set the LED5 pin mode
    pinMode(RELAY1_PIN, OUTPUT);      // set the RELAY1 pin mode
    pinMode(RELAY2_PIN, OUTPUT);      // set the RELAY2 pin mode
    digitalWrite(LED4_PIN,HIGH);
    digitalWrite(LED5_PIN,HIGH);
    digitalWrite(RELAY1_PIN,LOW);
    digitalWrite(RELAY2_PIN,LOW);

    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin((char*)ssid,(char*)password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        digitalWrite(LED4_PIN,!digitalRead(LED4_PIN));
        Serial.print(".");
    }
    digitalWrite(LED4_PIN,HIGH);
    digitalWrite(LED5_PIN,LOW);

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
}

void loop()
{
  bool isContentLen;
  bool isHeaderEnd;
  bool isNewLine;
  bool isStartData;
  uint32_t dataLen;
  String currentLine;
  String strDataLen;
	WiFiClient client = server.available();   // listen for incoming clients

	if (client)
	{                                         // if you get a client,
		Serial.println("new client");           // print a message out the serial port
		currentLine="";
    strDataLen="";
    dataLen=0;
    isContentLen=false;
    isHeaderEnd=false;
    isNewLine=false;
    isStartData=false;
		while (client.connected())
		{                                       // loop while the client's connected
		  if (client.available())
		  {                                     // if there's bytes to read from the client,
  			char c = client.read();             // read a byte, then
  			Serial.write(c);                    // print it out the serial monitor
        digitalWrite(LED4_PIN,!digitalRead(LED4_PIN));

        if(c!='\r' && c!='\n')
        {
          //Add char.
          currentLine+=c;
          //Set content length string.
          if(isContentLen)
            strDataLen+=c;
          //Set data counter.
          if(isStartData)
          {
            if(dataLen)
            {
              dataLen--;
              if(!dataLen)
                isNewLine=true;
            }
          }
        }
        else if(c=='\n')
        {
          isNewLine=true;
          if(currentLine.length()==0)
            isHeaderEnd=true;
        }

        #if (defined(_POST_REC))
        //Check content length.
        if(currentLine.endsWith("Content-Length: "))        
          isContentLen=true;
        #endif

        #if (defined(_GET_REC))
          parseCurrentLine(&currentLine);
        #else
          #if (defined(_POST_REC))
          //Parse current line.
          if(isHeaderEnd)      //For process only POST parameters.
            parseCurrentLine(&currentLine);
          #endif
        #endif
        
        if(isNewLine)
        {
          isNewLine=false;
          if(isContentLen)
          {
            isContentLen=false;
            dataLen=strDataLen.toInt();
            strDataLen="";
          }
          else if(isHeaderEnd)
          {
            if(dataLen)
              isStartData=true;
            else
            {
			        //Exec operation and exit loop.
              exOperations(&client);
              break;
            }
          }
          currentLine="";
        }
		  }
		}
		// close the connection:
		client.stop();
		Serial.println("client disonnected");
	}
}
