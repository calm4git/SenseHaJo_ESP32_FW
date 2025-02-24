#include "pumptask.h"
#include "../jsoninterface/jsoninterface.h"

enum emPumpFSMstate{
    PumpIdle=0,
    PumpConvey,
    PumpError,
};

TaskHandle_t xHandlePump = NULL;
SemaphoreHandle_t mtxPumpduration=NULL;

static uint32_t pumpduration=0;
static emPumpFSMstate fsmstate;
static pump_config_t pumpconfig;

struct PumpTaskParams {
    SenseEHajoGPIOPins* pins;
    pinname pumppin;
    pinname reedwaterempty;
};

typedef struct  {
    xTaskHandle handle;
    emPumpEvent event;
    uint32_t mask;
} pump_evententry_t;

typedef struct  {
    xTaskHandle handle;
    emPumpErrorEvent event;
    uint32_t mask;
} pump_error_evententry_t;

pump_evententry_t pump_evententrytable[32];
pump_error_evententry_t pump_error_evententrytable[32];

void pumptask( void* params);

void StartPumpTask( SenseEHajoGPIOPins* pins, pinname pumppin, pinname reedwaterempty ){
    //StaticMessageBuffer_t   xMessageBuffer = xMessageBufferCreate( xMessageBufferSizeBytes );
    mtxPumpduration = xSemaphoreCreateMutex();
    PumpTaskParams params ;
    assert(pins != NULL);
    params.pins = pins;
    params.pumppin = pumppin;
    params.reedwaterempty = reedwaterempty;
    
    if(pdPASS  != xTaskCreate( pumptask, "PumpTask", 8192, (void*)&params, tskIDLE_PRIORITY+2, &xHandlePump )){
        /* Fail */
    } else {
        /* Task up and running */
        configASSERT( xHandlePump );
    }

}

bool RegisterForPumpEvent(xTaskHandle handle, emPumpEvent Event, uint32_t eventmask ){    
    for(auto entry:pump_evententrytable ){
        if( (entry.handle == handle ) && (entry.event==Event ) ){
            /* We have already an entry in the table and return false */
            return false; /* early return */
        }
    }
    /* No entry found in table, if we have a NULL it means entry is empty */
    for(uint32_t i=0; i<( sizeof(pump_evententrytable)/sizeof(pump_evententrytable[0]) );i++){
        if( pump_evententrytable[i].handle == nullptr){
            pump_evententrytable[i].handle = handle;
            pump_evententrytable[i].event = Event;
            pump_evententrytable[i].mask = eventmask;    
            return true;
        }
    }

    return false;
}

bool UnRegisterForPumpEvent( xTaskHandle handle, emPumpEvent Event){
    for(uint32_t i=0; i<( sizeof(pump_evententrytable)/sizeof(pump_evententrytable[0]) );i++){
         if( (pump_evententrytable[i].handle == handle ) && (pump_evententrytable[i].event==Event ) ){
            pump_evententrytable[i].handle = nullptr;
            pump_evententrytable[i].mask = 0;    
            return true;
        }
    }
    return false;
}

void emittPumpEvent(emPumpEvent Event){
    for(uint32_t i=0; i<( sizeof(pump_evententrytable)/sizeof(pump_evententrytable[0]) );i++){
         if( (pump_evententrytable[i].handle != NULL ) && (pump_evententrytable[i].event==Event ) ){
            xTaskNotify(pump_evententrytable[i].handle,pump_evententrytable[i].mask,eSetBits);
        }
    }
}

bool RegisterForPumpErrorEvent(xTaskHandle handle, emPumpErrorEvent Event, uint32_t eventmask ){
    for(auto entry:pump_error_evententrytable ){
        if( (entry.handle == handle ) && (entry.event==Event ) ){
            /* We have already an entry in the table and return false */
            return false; /* early return */
        }
    }
    /* No entry found in table, if we have a NULL it means entry is empty */
    for(uint32_t i=0; i<( sizeof(pump_error_evententrytable)/sizeof(pump_error_evententrytable[0]) );i++){
        if( pump_error_evententrytable[i].handle == nullptr){
            pump_error_evententrytable[i].handle = handle;
            pump_error_evententrytable[i].event = Event;
            pump_error_evententrytable[i].mask = eventmask;    
            return true;
        }
    }

    return false;
}

bool UnRegisterForPumpErrorEvent( xTaskHandle handle, emPumpErrorEvent Event){
    for(uint32_t i=0; i<( sizeof(pump_error_evententrytable)/sizeof(pump_error_evententrytable[0]) );i++){
        if( (pump_error_evententrytable[i].handle == handle ) && (pump_error_evententrytable[i].event==Event ) ){
            /* do a bzero of the whole element */
            bzero(&pump_error_evententrytable, sizeof(pump_error_evententrytable[0]) );
            return true;
        }
    }
    return false;
}

void emittPumpErrorEvent(emPumpErrorEvent Event){
    for(uint32_t i=0; i<( sizeof(pump_evententrytable)/sizeof(pump_evententrytable[0]) );i++){
         if( (pump_error_evententrytable[i].handle != NULL ) && (pump_error_evententrytable[i].event==Event ) ){
            xTaskNotify(pump_error_evententrytable[i].handle,pump_error_evententrytable[i].mask,eSetBits);
        }
    }
}

bool conveywater( uint32_t amount,emPumpConveyUnit unit ){
    switch(unit){
        case emPumpConveyUnit::PumpConveySeconds:{
             if( xSemaphoreTake( mtxPumpduration, portMAX_DELAY )){
                pumpduration = amount;
                xSemaphoreGive(mtxPumpduration);
             }
        } break;

        case emPumpConveyUnit::PumpConveyMilliLiter:{
            if( xSemaphoreTake( mtxPumpduration, portMAX_DELAY )){
                pumpduration = amount*pumpconfig.timeToMilliLiter; /* do calculation from ml to s */
                xSemaphoreGive(mtxPumpduration);
             }
        } break;
    }

    return false;
}

bool conveyabort( void ){
    if( xSemaphoreTake( mtxPumpduration, portMAX_DELAY )){
        pumpduration=0;
        xSemaphoreGive(mtxPumpduration);
    }
    return true;
}

bool conveyinprogress( void ){
    if((pumpduration>0) && (hasError()==false)){
        return true;
    } else {
        return false;
    }
}

emPumpErrorEvent hasError(void){
    if(fsmstate == emPumpFSMstate::PumpError){
        return PumpOutOfWater;
    } else {
        return PumpNoError;
    }
}

void pumptask( void* params){
    /* Setup pump */
    fsmstate = emPumpFSMstate::PumpIdle; //Make visible to set flags for state
    PumpTaskParams parameter = *((PumpTaskParams*)params);
    /* we will check pump parmaeter in a 500ms raster */
    pumpconfig = readPumpConfig();

    while(1){
        uint32_t duration=0;
        
        uint32_t ulNotificationValue=0;
        ulNotificationValue = ulTaskNotifyTake( true, 500); /* we block here for max 500ms */
        if( xSemaphoreTake( mtxPumpduration, portMAX_DELAY )){
            duration =  pumpduration; 
            xSemaphoreGive(mtxPumpduration);
        }
        
        switch(fsmstate){
            case emPumpFSMstate::PumpIdle:{
                
                if(duration>0){
                   /* we should be in convery state */
                   fsmstate = emPumpFSMstate::PumpConvey;
                   emittPumpEvent(emPumpEvent::PumpingStart);
                } 
                digitalWrite(parameter.pumppin,LOW);

            } break;

            case emPumpFSMstate::PumpConvey:{
                
                if(duration>0){
                /* check if there is water left */
                

                    /* if we are out of water go to error */
                    if(LOW == digitalRead(parameter.reedwaterempty)){
                        emittPumpErrorEvent(emPumpErrorEvent::PumpOutOfWater);
                        fsmstate = emPumpFSMstate::PumpError;
                    } else {
                        if( xSemaphoreTake( mtxPumpduration, portMAX_DELAY )){
                            if(pumpduration>500){
                                 pumpduration=pumpduration-500;
                            } else {
                                pumpduration=0;
                            }
                            duration = pumpduration;
                            xSemaphoreGive(mtxPumpduration);
                        }
                        if(duration>0){
                            digitalWrite(parameter.pumppin,HIGH);
                        } else {
                            digitalWrite(parameter.pumppin,LOW);
                        }
                    }
                } else {
                    emittPumpEvent(emPumpEvent::PumpingDone);
                    fsmstate = emPumpFSMstate::PumpIdle;
                }

            } break;

            case emPumpFSMstate::PumpError:{
                digitalWrite(parameter.pumppin,LOW);
                if(HIGH == digitalRead(parameter.reedwaterempty)){
                     fsmstate = emPumpFSMstate::PumpIdle;
                }
            } break;
        }

        

    }
}

