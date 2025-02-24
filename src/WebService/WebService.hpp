#ifndef __WEBSERVICE_HPP__
    #define __WEBSERVICE_HPP__

#include "ESPAsyncWebServer.h"

class WebService {
    public:
        WebService(void);
        ~WebService();
        void setup();
        void loop();
        void WebSocketUpdateBoilertemp(float value);
        

    private:
        struct keyevent{
            TaskHandle_t taskhandle;
            uint8_t eventbit;
        };
        
        keyevent KeyEvents[3];
        SemaphoreHandle_t mtxBoilertemp=NULL;
        SemaphoreHandle_t semBoilertemp=NULL;
        float Boilertemp=0;  

        bool started=false;
        void FileNotFound( AsyncWebServerRequest *request );
        void Config_JSON_POST(AsyncWebServerRequest *request);
        void Config_JSON_GET(AsyncWebServerRequest *request);
        void WSonEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
        void parseJson(uint8_t* data, uint32_t len);
        void emittKeyEvent(TaskHandle_t xHandle, uint8_t eventbit);
        void WebSocketSendBoilertemp(float value);


};

#endif