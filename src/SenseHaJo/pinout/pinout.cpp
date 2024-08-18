#include "pinout.hpp"
#include <Arduino.h>

/* Table can later be loaded via JSON file to dynamically support different HW revisions (maybe)*/
pinconfig_t pinconfig [pinname::PINNAME_CNT]={
    [SWITCH1] = {.pin=23}, 
    [SWITCH2] = {.pin=26},
    [SWITCH3] = {.pin=27},
    /*[DISP_BTN] = {.pin=18}, */
    [MCU_RX] = {.pin=3}, 
    [MCU_TX] = {.pin=1},
    [WS2812_IO] = {.pin=18},
    [PUMP] = {.pin=5},
    [HEATER] = {.pin=19},
    [REED] = {.pin=13},
    [REED2] = {.pin=34},
    [HALSENSOR]= {.pin=21},
    [I2C_DISP_SCL] = {.pin=23},
    [I2C_DISP_SDA] = {.pin=22}, 
    [ANALOG_IN] = {.pin=36}
    /*, */
    /*
    [BL0937_CF1] = {.pin=35}, 
    [BL0937_CF] = {.pin=34}, 
    [BL0937_SEL] = {.pin=32}, 
    [MAINS_HALFWAVE] = {.pin=16}
    */
};
    
    SenseEHajoGPIOPins::SenseEHajoGPIOPins( ){
       
    }
    void SenseEHajoGPIOPins::Setup(){
        digitalin.push_back(SWITCH1);
        digitalin.push_back(SWITCH2);
        digitalin.push_back(SWITCH3);
        //digitalin.push_back(DISP_BTN);
        digitalin.push_back(REED);
        digitalin.push_back(REED2);
        digitalin.push_back(HALSENSOR);
        /*
        digitalin.push_back(MAINS_HALFWAVE);
        digitalin.push_back(BL0937_CF);
        digitalin.push_back(BL0937_CF1);
        */
        
        for(auto inpin: digitalin){
            pinMode(pinconfig[inpin].pin, INPUT);
        }

        digitalout.push_back(HEATER);
        digitalout.push_back(PUMP);
        //digitalout.push_back(BL0937_SEL);
        
        for(auto outpin:digitalout){
            pinMode(pinconfig[outpin].pin,OUTPUT);
        }

        /* Set Heater and Pump to OFF */
        digitalWrite(pinconfig[HEATER].pin, LOW);
        digitalWrite(pinconfig[PUMP].pin, LOW);

        analogpins.push_back(ANALOG_IN);
        analogin.Setup((const std::vector<pinname>*)(&analogpins), SenseEHajoAnalogIn::emResolution::_12Bits);
    }

    SenseEHajoGPIOPins::~SenseEHajoGPIOPins( ){
        /* Nothing to do here */
    }

    bool SenseEHajoGPIOPins::GetPinStatus(pinname pin){
        /* We can read every pin as digital in, even if configured as something else */
        return digitalRead(pinconfig[pin].pin);
    }

    void SenseEHajoGPIOPins::SetPinStatus(const pinname pin, const bool state){
        for(auto output:digitalout){
            if(pin == output){
                digitalWrite(pinconfig[pin].pin,state);
                break;
            }
        }
    }

    const std::vector<pinname>* SenseEHajoGPIOPins::GetAnalogPins(void){
        return (const std::vector<pinname>*)(&analogpins);
    }

    SenseEHajoAnalogIn::SenseEHajoAnalogIn(){
      /* Nothing to do */
    }

    SenseEHajoAnalogIn::analogvalue SenseEHajoAnalogIn::ReadAnalogIn(pinname pin){
        analogvalue value;
        value.status = NotAnalogPin;
        value.value = 0;
        if(nullptr != _analogpinptr ){
            for(auto pinenum:*_analogpinptr){
                if(pinenum == pin){
                    value.status = Okay;
                    value.value = analogRead(pinconfig[pin].pin);
                    break;
                }
            }
        }
        return value;
    }

    void SenseEHajoAnalogIn::Setup(const std::vector<pinname>* pinsptr, emResolution resolution){
          switch(resolution){
            case _9Bits:{
                analogReadResolution(9);
            }break;

            case _10Bits:{
                analogReadResolution(10);    
            }break;

            case _11Bits:{
                analogReadResolution(11);    
            }break;

            case _12Bits:{
                analogReadResolution(12);    
            }break;

            default:{
                analogReadResolution(12);    
            }break;
        }
        _analogpinptr = pinsptr;
     }