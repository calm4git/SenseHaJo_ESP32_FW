#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "ota.hpp"

ota::ota(){
    
}

ota::~ota(){
    ArduinoOTA.end();
}

void ota::setHostname(String hostname){
    this->hostname = hostname;
}

void ota::setPassword(String password){
    this->password = password;
}
    

void ota::loop(){
    if(true==WiFi.isConnected()){
        if(false == this->started){
            Serial.print("Start OTA service");
            if(hostname.length()>0){
                ArduinoOTA.setHostname(hostname.c_str());
            }

            if(password.length()>0){
                ArduinoOTA.setPassword(password.c_str());
            }
            ArduinoOTA.begin();
            this->started=true;
        } else {
            ArduinoOTA.handle();
        }
    }
}