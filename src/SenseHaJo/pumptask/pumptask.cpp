#include "pumptask.h"
TaskHandle_t xHandlePump = NULL;
enum emPumpFSMstate{
    PumpIdle=0,
    PumpConvey,
    PumpError,
};

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
    PumpTaskParams params ;
    assert(pins != NULL);
    params.pins = pins;
    params.pumppin = pumppin;
    params.reedwaterempty = reedwaterempty;
    
    if(pdPASS  != xTaskCreate( pumptask, "PumpTask", 4096, (void*)&params, tskIDLE_PRIORITY+2, &xHandlePump )){
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


bool conveywater( uint32_t amount,emPumpConveyUnit unit ){
    switch(unit){
        case emPumpConveyUnit::PumpConveySeconds:{

        } break;

        case emPumpConveyUnit::PumpConveyMilliLiter:{

        } break;
    }

    return false;
}

bool conveyabort( void ){
    return false;
}

bool conveyinprogress( void ){
    return false;
}

emPumpErrorEvent hasError(void){
    return PumpNoError;
}

void pumptask( void* params){
    /* Setup pump */
    emPumpFSMstate fsmstate = emPumpFSMstate::PumpIdle;

    /* we will check pump parmaeter in a 500ms raster */
    while(1){
        uint32_t ulNotificationValue=0;
        ulNotificationValue = ulTaskNotifyTake( true, 500); /* we block here for max 500ms */
        


    }
}