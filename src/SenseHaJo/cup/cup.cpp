/* we will define one cup and multiple ones */
/* settings shall be stored in a JSON in out filesystem */
#include <ArduinoJson.h>
#include "FS.h"
#include <LittleFS.h>
#include "cup.h"

        coffeeCup::coffeeCup(void){
            _filename ="";
            _volume=0;
        }
        coffeeCup::~coffeeCup(void){

        }

        /**
         * we expect a JSON like this
         * {
         *  "volume" : 200,
         * }
         */

        bool coffeeCup::loadSettings(String filename){
            File file = LittleFS.open(filename.c_str(), "r");
            if (!file) {
                _volume;
                saveSettings(filename); /* we generate a new cup if load fails */
                return false;
            }
            StaticJsonDocument<256> doc;
            DeserializationError error = deserializeJson(doc, file);
            _volume = doc["volume"].as<uint32_t>();
            file.close();
            return true;

        } /* We save data to LittleFS */

        bool coffeeCup::saveSettings(String filename){
            File file = LittleFS.open(filename, "w");
            if (!file) {
                return false;
            }   
            StaticJsonDocument<256> doc;
            // Set the values in the document
            doc["volume"] = _volume;
            if (serializeJson(doc, file) == 0) {
                file.close();
                return false;
            }
            file.close();
            return true;
        }
        String coffeeCup::GetLoadedFilename(void){
            return _filename;
        }

        uint32_t coffeeCup::getVolume(void){
            if(true==_isDouble){
                if(_volume < (UINT32_MAX/2)){
                    return (_volume*2);
                } else {
                    return UINT32_MAX;
                }
            } else {
                return (_volume);
            }
        }
        void coffeeCup::setVolume(uint32_t volumeMilliLiter){
            _volume = volumeMilliLiter;
        }
        
        /* Non persiten settings */
        bool coffeeCup::isDoubleVolume(void){
            return _isDouble;
        }

        void coffeeCup::setDoubleVolume(bool doubleVolume){
            _isDouble = doubleVolume;
        }

