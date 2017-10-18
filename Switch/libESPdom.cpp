#include "libESPdom.h"

void libESPdom::sendRequestHttp(WiFiClient *client,char *params,te_HTTP_METHOD httpMethod)
{
	uint32_t lenParams;
	uint32_t sentParams;
	IPAddress ip;
	
	if(httpMethod==HTTP_METHOD_GET)
	{
		client->println("GET /?");
		
		//Send params (already formatted for send):
		lenParams=strlen(params);
		sentParams=0;
		while(sentParams<lenParams)
			sentParams+=client->print(params+sentParams);
		
		client->println(" HTTP/1.1");
		
		client->print("HOST ");
		client->println(WiFi.localIP());
		
		client->println("Content-type:text/html");
	}
	else if(httpMethod==HTTP_METHOD_POST)
	{
		client->println("POST / HTTP/1.1");
		
		client->print("HOST ");
		client->println(WiFi.localIP());
		
		client->println("Content-type:text/html");
		client->println();
		
		//Send params (already formatted for send):
		lenParams=strlen(params);
		sentParams=0;
		while(sentParams<lenParams)
			sentParams+=client->print(params+sentParams);
	}

	// The HTTP request ends with another blank line:
	client->println();

	Serial.println("HTML request sent");           // print a message out the serial port
}

void libESPdom::sendResponseHttp(WiFiClient *client,char *httpBuf)
{
	uint32_t lenHtmlBuf;
	uint32_t sentHtmlBuf;

	// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
	// and a content-type so the client knows what's coming, then a blank line:
	client->println("HTTP/1.1 200 OK");
	
	client->println("Content-type:text/html");
	
	client->println();

	// the content of the HTTP response follows the header:
	lenHtmlBuf=strlen(httpBuf);
	sentHtmlBuf=0;

	while(sentHtmlBuf<lenHtmlBuf)
		sentHtmlBuf+=client->print(httpBuf+sentHtmlBuf);

	// The HTTP response ends with another blank line:
	client->println();

	Serial.println("HTML response sent");           // print a message out the serial port
}

void libESPdom::sendReadVObject(WiFiClient *client,char *vObjectId)
{
	char strHtml[300];

	memset(strHtml,'\0',sizeof(strHtml));

	strcat(strHtml,"ORD&");
	strcat(strHtml,vObjectId);

	this->sendRequestHttp(client,strHtml,HTTP_METHOD_POST);
}

void libESPdom::sendWriteVObject(WiFiClient *client,char *vObjectId,char *value)
{
	char strHtml[300];

	memset(strHtml,'\0',sizeof(strHtml));

	strcat(strHtml,"OWR&");
	strcat(strHtml,vObjectId);
	strcat(strHtml,"=");
	strcat(strHtml,value);

	this->sendRequestHttp(client,strHtml,HTTP_METHOD_POST);
}

void libESPdom::sendWriteVObject(WiFiClient *client,char *vObjectId,uint32_t value)
{
	char strHtml[300];
	char strInt[11];

	memset(strHtml,'\0',sizeof(strHtml));
	memset(strInt,'\0',sizeof(strInt));

	strcat(strHtml,"OWR&");
	strcat(strHtml,vObjectId);
	strcat(strHtml,"=");
	strcat(strHtml,strInt);

	this->sendRequestHttp(client,strHtml,HTTP_METHOD_POST);
}

void libESPdom::sendValueVObject(WiFiClient *client,char *vObjectId,char *value)
{
	char strHtml[600];

	memset(strHtml,'\0',sizeof(strHtml));

	strcat(strHtml,"{\"");
	strcat(strHtml,vObjectId);
	strcat(strHtml,"\":{\"value\":\"");
	strcat(strHtml,value);
	strcat(strHtml,"\"}}");

	this->sendResponseHttp(client,strHtml);
}

void libESPdom::sendValueVObject(WiFiClient *client,char *vObjectId,uint32_t value)
{
	char strHtml[600];
	char strInt[11];

	memset(strHtml,'\0',sizeof(strHtml));
	memset(strInt,'\0',sizeof(strInt));

	sprintf(strInt,"%d",value);

	strcat(strHtml,"{\"");
	strcat(strHtml,vObjectId);
	strcat(strHtml,"\":{\"value\":\"");
	strcat(strHtml,strInt);
	strcat(strHtml,"\"}}");

	this->sendResponseHttp(client,strHtml);
}
