#ifndef __WEBSERVICE_HPP__
    #define __WEBSERVICE_HPP__

#include "ESPAsyncWebServer.h"
class WebService {
    public:
        WebService(void);
        ~WebService();
        void loop();

    private:
        bool started=false;
        void FileNotFound( AsyncWebServerRequest *request );
        void Config_JSON_POST(AsyncWebServerRequest *request);
        void Config_JSON_GET(AsyncWebServerRequest *request);
        void WSonEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
};

#endif