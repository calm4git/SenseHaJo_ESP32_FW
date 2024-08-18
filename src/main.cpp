#include <Arduino.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "FS.h"
#include "LittleFS.h"

#include "Display/Display.h"
#include "LED/led.hpp"
#include "OTA/ota.hpp"
#include "WebService/WebService.hpp"
#include "MQTT/mqtt_client.h"
#include "SenseHaJo/sensehajo.h"


ota updater;
WiFiManager wm;
WebService webservice;
mqtt_client mqtt;
led leds;
void setup() {
  //Basic setup for hardware
  Serial.begin(115200);
  if (!LittleFS.begin(true)){
    Serial.println("LittleFS Mount Failed");
  }
  #ifdef TFT_eSPI_DISPLAY
    DisplayInit();
  #endif

  leds.init();
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP    

  wm.setConfigPortalBlocking(false);
  wm.setConfigPortalTimeout(120); //We keep the portal running for 2 minutes
    //automatically connect using saved credentials if they exist
    //If connection fails it starts an access point with the specified name
  if(wm.autoConnect(String("SenseHaJo_"+WiFi.macAddress()).c_str() )){
      Serial.println("WiFi connection restored");
  } else {
      Serial.println("Configportal running");
  }
  /* If we are in config portal mode, we will start a second webserver instance on another port for WEB UI */ 
  sensehajo(&leds, &mqtt, &webservice);
  /* prepartion to track the runtime of our main loop */
  pinMode(14,OUTPUT);
  digitalWrite(14,LOW);
}

/* Loop inside the main task */
void loop() {
  //This rund within main thread 
  webservice.loop();
  updater.loop();
  wm.process();
  mqtt.loop();
  #ifdef TFT_eSPI_DISPLAY
    DisplayLoop();
  #endif
  leds.loop();
}




