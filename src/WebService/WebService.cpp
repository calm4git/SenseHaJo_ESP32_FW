#include "AsyncJson.h"
#include "ArduinoJson.h"

#include <DNSServer.h>

#ifdef ESP32
    #include <WiFi.h>
    #include <AsyncTCP.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESPAsyncTCP.h>
#endif

#include "WebService.hpp"
#include "LittleFS.h"

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

WebService::WebService(void){

}


WebService::~WebService(){
    server.end();
}

void WebService::FileNotFound(AsyncWebServerRequest *request ){
    fs::File  error=LittleFS.open("/404.htm");
    if(false==error.available()){
        String message = "File Not Found\n\n";
        message += "URL: ";
        message += request->url();
        message += "\nMethod: ";
        message += (request->method() == HTTP_GET) ? "GET" : "POST";
        message += "\nArguments: ";
        message += request->args();
        message += "\n";
        for (uint8_t i = 0; i < request->args(); i++) {
        message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
        }
        request->send(404, "text/plain", message);
    } else {
        //Send index.htm with default content type
        request->send(LittleFS, "/404.htm","text/html");    
    }
}

void WebService::Config_JSON_POST(AsyncWebServerRequest *request){
    int params = request->params();
    for(int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        
      } else if(p->isPost()){
        
      } else {
       
      }
    }

    if(request->hasParam("body", true))
    {
      AsyncWebParameter* p = request->getParam("body", true);
      JsonDocument doc;
     
      if(DeserializationError::Code::Ok == deserializeJson(doc, p->value()))
      {
        /* Check JSON for content */

        /* Set new values to config */
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{'msg':'done'}");
        request->send(response);
      }
      else
      {
        AsyncWebServerResponse *response = request->beginResponse(400, "application/json", "{'msg':'Could not parse JSON'}");
        request->send(response);
      }
    }
    else
    {
      AsyncWebServerResponse *response = request->beginResponse(400, "application/json", "{'msg':'No body'}");
      request->send(response);
    }
}

void WebService::Config_JSON_GET(AsyncWebServerRequest *request){
    //Prepare new config JSON (Simple direct version up tp 4k data)
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    JsonDocument  jsonDocument; //Limit to 4k 
    JsonObject root = jsonDocument.to<JsonObject>();
    root["heap"] = ESP.getFreeHeap();
    root["ssid"] = WiFi.SSID();
    serializeJson(jsonDocument,*response);
    request->send(response);
}

void WebService::WSonEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
    switch(type){
        case WS_EVT_CONNECT:{
            client->printf("Hello Client %u :)", client->id());
            client->ping();
        }break;
        
        case WS_EVT_DISCONNECT:{
            //client disconnected
            //os_printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
        }break;
        
        case WS_EVT_ERROR:{
            //error was received from the other end
            //os_printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
        }break;
        
        case WS_EVT_PONG:{
            //pong message was received (in response to a ping request maybe)
            //os_printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
        }break;
        
        case WS_EVT_DATA:{
            AwsFrameInfo * info = (AwsFrameInfo*)arg;
            if(info->final && info->index == 0 && info->len == len){
                //the whole message is in a single frame and we got all of it's data
                //os_printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
                switch(info->opcode){
                    case WS_CONTINUATION:{

                    } break;
                    case WS_TEXT:{
                        data[len] = 0;
                        //os_printf("%s\n", (char*)data);
                        client->text("I got your text message");
                    }break;
                    case WS_BINARY:{
                        for(size_t i=0; i < info->len; i++){
                            //os_printf("%02x ", data[i]);
                        }
                        //os_printf("\n");
                        client->binary("I got your binary message");
                    }break;

                    case WS_DISCONNECT:{

                    } break;

                    case WS_PING: {

                    }break;

                    case WS_PONG:{

                    } break;

                    default:{

                    }break;

                }
            } else {
                //message is comprised of multiple frames or the frame is split into multiple packets
                if(info->index == 0){
                    if(info->num == 0){
                        //os_printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                    }
                    //os_printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
                }
                //os_printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
                if(info->message_opcode == WS_TEXT){
                    data[len] = 0;
                    //os_printf("%s\n", (char*)data);
                } else {
                    for(size_t i=0; i < len; i++){
                        //os_printf("%02x ", data[i]);
                    }
                    //os_printf("\n");
                }

                if((info->index + len) == info->len){
                    //os_printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
                    if(info->final){
                        //os_printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                        if(info->message_opcode == WS_TEXT){
                            client->text("I got your text message");
                        } else {
                            client->binary("I got your binary message");
                        }
                    }
                }
                
            }
      
        } break;
        
        default:{
            
        }break;
    }
}
    
void WebService::loop(){
    static uint32_t lastrun=millis();
    if(true==WiFi.isConnected()){
        if(false==this->started){
           
            /* Here we need to register our paths that allow for POST */
            server.on("/settings/config.json",HTTP_POST,std::bind(&WebService::Config_JSON_POST, this, std::placeholders::_1));
            server.on("/settings/config.json",HTTP_GET,std::bind(&WebService::Config_JSON_GET, this, std::placeholders::_1));
            /* Configure WS Plugin */
            ws.onEvent(std::bind(&WebService::WSonEvent, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5,std::placeholders::_6 ));
            server.addHandler(&ws);

            // serve all static files
            server.serveStatic("/", LittleFS, "/");
            // handle cases when file is not found
            server.onNotFound(std::bind(&WebService::FileNotFound,this, std::placeholders::_1)); 
            server.begin();

            lastrun=millis();
            this->started=true;
        } else {
           //No need to do someting in the loop as the server runs in his own thread (will likly cause problems later ?!)
            if( (millis()-lastrun)>1000){
               ws.cleanupClients();
               lastrun=millis(); 
            }
        }
    }
}
