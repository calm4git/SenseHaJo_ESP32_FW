#include "Arduino.h"
#include "boilertempreg.h"
#include "../ntc10k/ntc10k.hpp"
#include "boilertemplimit.hpp"


static float current_temp=0;
static float targettemp=0;
static SenseEHajoGPIOPins* pinptr=NULL;
static pinname pin;
static pinname adcpin;
static TaskHandle_t xHandle = NULL;
boilertemplimits limit(0,120); /* 0 - 120 degree */
BoilerTempQHandle_t registeredQueus[16];

void start_Boilertempreg(SenseEHajoGPIOPins* pptr, pinname p, pinname adcp){
    assert(pptr != NULL);
    pinptr = pptr;
    pin = p;
    adcpin = adcp;
    pinptr->SetPinStatus(pin,false);
    /* Start task */
    if(pdPASS  != xTaskCreate( &Boilertempreg_task, "BoilerTempRegtask", 4096, nullptr, tskIDLE_PRIORITY+4, &xHandle ))
    {
        /* Fail */
    } else {
        /* Task up and running */
        configASSERT( xHandle );
    }
}

bool registerDataQueue(BoilerTempQHandle_t qhandle, uint32_t elementsize){
    bool registered=false;
    int32_t free_slot=-1;
    for(uint32_t i=0;i<( (sizeof(registeredQueus)/sizeof(registeredQueus[0]) ));i++ ){
        if(qhandle==registeredQueus[i]){
            return true;
        }
        if((free_slot<0) && (NULL == registeredQueus[i])){
            free_slot=i;
        }
        
    }

    if(free_slot>=0){
        registeredQueus[free_slot]=qhandle;
        return true;
    } else {
        return false;
    }

}

float getBoilerTargetTemp(){
    return targettemp;
}
void setBoilerTargetTemp(float temp){
    /* we will limit the max temp here to safe limits */
    targettemp=temp;
}
float getBoilerCurrentTemp(void){
    return current_temp; /* last known adc temp */
}
void Boilertempreg_task(void* param){
    NTC10K ntc((float)3300, (float)10000, (float)25, (float)3950, (int)4095);
    
    while(1==1){
        SenseEHajoAnalogIn::analogvalue value = pinptr->analogin.ReadAnalogIn(adcpin);
        //printf("Analog value: %u\r\n", value.value);
        float temp = ntc.convertADCToTemperature(value.value);
        //printf("Analog temp: %f\r\n", temp);
        current_temp = temp;
        if(targettemp>current_temp){
            pinptr->SetPinStatus(pin,true);
        } else {
            pinptr->SetPinStatus(pin,false);
        }
        tempdata_t data;
        data.value = current_temp;
        for(uint32_t i=0;i<( (sizeof(registeredQueus)/sizeof(registeredQueus[0]) ));i++ ){
            if(NULL!=registeredQueus[i]){
                #ifdef __useringbuf_h__
                    UBaseType_t res =  xRingbufferSend(registeredQueus[i], &data, sizeof(data),0);
                    if (res != pdTRUE) { 

                    }
                #else
                    size_t byteswritten = xMessageBufferSend(registeredQueus[i],&data, sizeof(data),0);
                    if(byteswritten != sizeof(data) {

                    }
                #endif
            }
        delay(100);      
    }
}