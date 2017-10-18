#ifndef __libESPdom__
#define __libESPdom__


#include <WiFi.h>

typedef enum _HTTP_METHOD
{
	HTTP_METHOD_GET,
	HTTP_METHOD_POST
}te_HTTP_METHOD;

class libESPdom
{
  public:
    void sendRequestHttp(WiFiClient *client,char *htmlBuf,te_HTTP_METHOD httpMethod);
    void sendResponseHttp(WiFiClient *client,char *htmlBuf);
    void sendReadVObject(WiFiClient *client,char *vObjectId);
    void sendWriteVObject(WiFiClient *client,char *vObjectId,char *value);
    void sendWriteVObject(WiFiClient *client,char *vObjectId,uint32_t value);
    void sendValueVObject(WiFiClient *client,char *vObjectId,char *value);
    void sendValueVObject(WiFiClient *client,char *vObjectId,uint32_t value);
};

#endif
