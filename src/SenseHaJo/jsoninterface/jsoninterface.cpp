#include "jsoninterface.h"
#include "FS.h"
#include "LittleFS.h"
#include "ArduinoJson.h"

static char output[2048]; //static keeps variable only accessible from within cpp file

coffecup_config_t readCoffeeCupConfig(void){
    File configfile = LittleFS.open("/coffeeconfig.json", "r", true);
    coffecup_config_t config;

    if(NULL == configfile){
        /* This is an error we gernerate a default file */
        config.coffeetemp = 80;
        config.cupsizeMilliLiter = 200;
        strncpy(config.cupcfgfile,"/defaultcup.json",sizeof("/defaultcup.json"));
        writeCoffeeCupConfig(config);
        
    } else {
        size_t size = configfile.size();
        if(size>2048){
            /* we will not read more than 2k files */
            config.coffeetemp = 80;
            config.cupsizeMilliLiter = 200;
            strncpy(config.cupcfgfile, "/defaultcup.json", sizeof("/defaultcup.json"));
        } else {
            
            std::unique_ptr<char[]> buf(new char[size]);
            configfile.readBytes(buf.get(), size);
            StaticJsonDocument<300> doc;
            auto error = deserializeJson(doc, buf.get());
            if(error){
                /* something went wrong */
                Serial.printf("Error serialize json: %i \n\r",error);
            } else {
                if(true==doc["targettemp"].is<float>()){
                    config.coffeetemp = doc["targettemp"];
                } else {
                    Serial.printf("targettemp not a FLOAT \n\r");
                    config.coffeetemp = 80; /* default value */
                }

                if(true==doc["cupvolume"].is<int>()){
                    if(doc["cupvolume"]>=0){
                        config.cupsizeMilliLiter=doc["cupvolume"];
                    } else {
                        Serial.printf("cupvolume less than 0 n\r");
                        config.cupsizeMilliLiter=50; // Shall we default to 50ml ?
                    }
                } else {
                    Serial.printf("cupvolume not a INT n\r");
                    config.cupsizeMilliLiter=50; // Shall we default to 50ml ?
                }

                if(true==doc["cupcfgfile"].is<const char*>()){
                    String filename = doc["cupcfgfile"];
                    strncpy(config.cupcfgfile,filename.c_str(),sizeof(config.cupcfgfile));
                } else {
                    Serial.printf("cupcfgfile not a CHAR* n\r");
                }

                if(false== LittleFS.exists(config.cupcfgfile)){
                    /* Non existing file */
                    Serial.printf("File '%s'not found, use /defaultcup.json \r\n",config.cupcfgfile );
                    strncpy(config.cupcfgfile, "/defaultcup.json", sizeof("/defaultcup.json"));
                    writeCoffeeCupConfig(config);
                }
                
                
            }
        } 
    }
    return config;
}

void writeCoffeeCupConfig(coffecup_config_t config){
    static char output[2048]; //static keeps this over calls and also forces this to be placed in RAM
    bzero(output, sizeof(output));
    File configfile = LittleFS.open("/coffeeconfig.json", "w",true);
    if(NULL == configfile){
        Serial.printf("Can't write /coffeeconfig.json");
        return;
    }
    JsonDocument doc;

    doc["targettemp"] = config.coffeetemp;
    doc["cupvolume"] = config.cupsizeMilliLiter;
    doc["cupcfgfile"] = config.cupcfgfile;

    

    doc.shrinkToFit();  // optional

    size_t size= serializeJson(doc, output);
    configfile.write((uint8_t*)(&output[0]),size);
    configfile.close();
    
}

pump_config_t readPumpConfig(void){
    File configfile = LittleFS.open("/pumpconfig.json", "r",true);
    pump_config_t config;

    if(NULL == configfile){
        /* This is an error we gernerate a default file */
       config.timeToMilliLiter = 1;
       writePumpConfig(config);
        
    } else {
        size_t size = configfile.size();
        if(size>2048){
            /* we will not read more than 2k files */
            config.timeToMilliLiter = 1;
        } else {
            
            std::unique_ptr<char[]> buf(new char[size]);
            configfile.readBytes(buf.get(), size);
            StaticJsonDocument<300> doc;
            auto error = deserializeJson(doc, buf.get());
            if(error){
                /* something went wrong */
                Serial.printf("Error serialize json: %i \n\r",error);
            } else {
                if(true==doc["timeToMilliLiter"].is<float>()){
                    config.timeToMilliLiter = doc["timeToMilliLiter"];
                } else {
                    Serial.printf("timeToMilliLiter not a float \n\r");
                    config.timeToMilliLiter = 1; /* default value */
                }
            }
        } 
    }
    return config;
}

void writePumpConfig(pump_config_t config){
    bzero(output, sizeof(output));
  
    File configfile = LittleFS.open("/pumpconfig.json", "w",true);
    if(NULL == configfile){
        Serial.println("Can't write pumpconfig.json");
        return;
    }
    JsonDocument doc;

    doc["timeToMilliLiter"] = config.timeToMilliLiter;
    
   
    doc.shrinkToFit();  // optional

    size_t size= serializeJson(doc, output);
    configfile.write((uint8_t*)(&output[0]),size);
    configfile.close();
}
    
machine_config_t readMachineConfig(void){
    File configfile = LittleFS.open("/machineconfig.json", "r",true);
    machine_config_t config;
    
    if(NULL == configfile){
        /* This is an error we gernerate a default file */
        config.idletime = 300;
        config.standbytemp=60;
        writeMachineConfig(config);
        
    } else {
        size_t size = configfile.size();
        if(size>2048){
            /* we will not read more than 2k files */
            config.idletime = 300;
            config.standbytemp=60;
        } else {
            
            std::unique_ptr<char[]> buf(new char[size]);
            configfile.readBytes(buf.get(), size);
            StaticJsonDocument<300> doc;
            auto error = deserializeJson(doc, buf.get());
            if(error){
                /* something went wrong */
                Serial.printf("Error serialize json: %i \n\r",error);
            } else {
                if(true==doc["standbytemp"].is<float>()){
                    config.standbytemp = doc["standbytemp"];
                } else {
                    Serial.printf("standbytemp not a float \n\r");
                    config.standbytemp = 60; /* default value */
                }

                if(true==doc["idletime"].is<int>()){
                    if(doc["idletime"]>=0){
                        config.idletime=doc["idletime"];
                    } else {
                        Serial.printf("idletime less than 0 n\r");
                        config.idletime=300; 
                    }
                } else {
                    Serial.printf("idletime not a INT n\r");
                    config.idletime=300; 
                }
                
            }
        } 
    }
    return config;
}

void writeMachineConfig(machine_config_t config){
    bzero(output, sizeof(output));
  
    File configfile = LittleFS.open("/machineconfig.json", "w", true);
    if(NULL == configfile){
        Serial.printf("Can't write machineconfig.json\n\r");
        return;
    }
    JsonDocument doc;

    doc["standbytemp"] = config.standbytemp;
    doc["idletime"] = config.idletime;


    doc.shrinkToFit();  // optional

    size_t size= serializeJson(doc, output);
    configfile.write((uint8_t*)(&output[0]),size);
    configfile.close();
}
