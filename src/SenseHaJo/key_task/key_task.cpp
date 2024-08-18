#include "Esp.h"
//#include "../pinout/pinout.hpp"
#include "key_task.hpp"
#include "key_class.hpp"


TaskHandle_t KeyTaskxHandle = NULL;

struct TaskParams {
    SenseEHajoGPIOPins* pins;
};

/*  We need a class of key objects that will handle the Key logic 
    as we have four(4) pins we are interrested in we will add the
    objects for these
*/ 

Key KeyOneCup(true); /* OneCup Key*/
Key KeyTwoCups(true); /* TwoCups Key*/
/* If you know, you know ...*/
Key KeyPower(true);

/* We need to limit the amount of handles that can register */
/* Limit to 32 entrys for the keys */
typedef struct  {
    xTaskHandle handle;
    Key::emKeyState event;
    uint32_t mask;
    Key::emMachineKeys machinekey;
} evententry_t;

typedef struct  {
    xTaskHandle handle;
    Key::emKeyState event;
    uint8_t bit;
    Key::emMachineKeys machinekey;
} eventinputentry_t;

evententry_t evententrytable[32];
eventinputentry_t eventinputentrytable[32];


bool RegisterForKeyEvent(xTaskHandle handle, Key::emKeyState Event, Key::emMachineKeys Key, uint32_t eventmask ){
    for(auto entry:evententrytable ){
        if( (entry.handle == handle ) && (entry.event==Event ) && (entry.machinekey==Key )){
            /* We have already an entry in the table and return false */
            return false; /* early return */
        }
    }
    /* No entry found in table, if we have a NULL it means entry is empty */
    for(uint32_t i=0; i<( sizeof(evententrytable)/sizeof(evententrytable[0]) );i++){
        if(evententrytable[i].handle == nullptr){
            evententrytable[i].handle = handle;
            evententrytable[i].event = Event;
            evententrytable[i].machinekey=Key;
            evententrytable[i].mask = eventmask;    
            return true;
        }
    }

    return false;
}

bool UnRegisterForKeyEvent( xTaskHandle handle, Key::emKeyState Event, Key::emMachineKeys Key){
    for(uint32_t i=0; i<( sizeof(evententrytable)/sizeof(evententrytable[0]) );i++){
         if( (evententrytable[i].handle == handle ) && (evententrytable[i].event==Event ) && (evententrytable[i].machinekey==Key )  ){
            evententrytable[i].handle = nullptr;
            evententrytable[i].mask = 0;    
            return true;
        }
    }
    return false;
}

void emittkeyevent(Key::emMachineKeys key, Key::emKeyState state){
    /* go through the table and infrom tasks if they are waiting */
    for(uint32_t i=0; i<( sizeof(evententrytable)/sizeof(evententrytable[0]) );i++){
        if( (evententrytable[i].machinekey == key) && 
            (evententrytable[i].event == state)     &&
            (evententrytable[i].handle != nullptr)
            ){
            //Send task notify
            xTaskNotify(evententrytable[i].handle,evententrytable[i].mask,eSetBits);
        }
    }
}

xTaskHandle RegisterKeyEventSource(xTaskHandle handle, Key::emKeyState Event, Key::emMachineKeys Key, uint8_t eventbit ){
    int32_t freeindex = INT32_MIN;
    if(eventbit>=32){
        return NULL;
    }

    for(uint32_t i=0; i<( sizeof(eventinputentrytable)/sizeof(eventinputentrytable[0]) );i++){
        if(NULL != eventinputentrytable[i].handle){
            if(eventbit == eventinputentrytable[i].bit ){
                return NULL;
            }
        } else {
            if(freeindex < 0){
                freeindex=i;
            }
        }     
    }
    
    if( (freeindex < 0) || (freeindex >= ( sizeof(eventinputentrytable)/sizeof(eventinputentrytable[0]) )) ) {
        return NULL;
    }
    
    eventinputentrytable[freeindex].bit = eventbit; 
    eventinputentrytable[freeindex].handle = handle; /* Used for debug and to make sure regered task will only allowed to unregister */
    eventinputentrytable[freeindex].event = Event;
    eventinputentrytable[freeindex].machinekey = Key;
    
    return KeyTaskxHandle;
}

xTaskHandle UnregisterKeyEventSource(xTaskHandle handle, Key::emKeyState Event, Key::emMachineKeys Key, uint8_t eventbit ){
    if(eventbit>=32){
        return NULL;
    }

    for(uint32_t i=0; i<( sizeof(eventinputentrytable)/sizeof(eventinputentrytable[0]) );i++){
        if(handle == eventinputentrytable[i].handle){
            if( (eventbit == eventinputentrytable[i].bit ) &&
                (Event == eventinputentrytable[i].event ) &&
                (Key == eventinputentrytable[i].machinekey ) ){
                    eventinputentrytable[i].bit = 0; 
                    eventinputentrytable[i].handle = NULL; /* Used for debug and to make sure regered task will only allowed to unregister */
                    eventinputentrytable[i].event = Key::emKeyState::emKeyStateCNT;
                    eventinputentrytable[i].machinekey = Key::emMachineKeys::emMachineKeysCNT;
                    return KeyTaskxHandle;
                }
        } 
        return NULL;
    }
    
    
    
    
    return NULL;
}


void KeyTask(void * pvParameters){
    /* grab pins*/
    pinMode(12,OUTPUT);
    
    TaskParams* params = (TaskParams*)(pvParameters);
    assert(params);
    SenseEHajoGPIOPins* pins = (SenseEHajoGPIOPins*)(params->pins);
    /* Init here...*/
    KeyTwoCups.UpdateKeyState(pins->GetPinStatus(pinname::SWITCH1));
    KeyPower.UpdateKeyState(pins->GetPinStatus(pinname::SWITCH2));
    KeyOneCup.UpdateKeyState(pins->GetPinStatus(pinname::SWITCH3));
    /* KeyDisplay.UpdateKeyState(pins->GetPinStatus(pinname::DISP_BTN)); */
    Serial.print("Start Key task");
    
    while(1==1){
        digitalWrite(12,HIGH);
        Key::keystate state = KeyTwoCups.UpdateKeyState(pins->GetPinStatus(pinname::SWITCH1));
        if(true == state.has_changed){
            /* Emitt new key state as event */
            emittkeyevent(Key::emMachineKeys::TwoCups, state.state );
            switch(state.state ){
                case Key::emKeyState::KeyState_Pressed:{
                    
                }break;

                case Key::emKeyState::KeyState_Released:{
                    
                }break;

                default:{
                    
                }
            }
            
        }
        
        state = KeyPower.UpdateKeyState(pins->GetPinStatus(pinname::SWITCH2));
        if(true == state.has_changed){
            /* Emitt new key state as event */
            emittkeyevent(Key::emMachineKeys::Power, state.state );
            switch(state.state ){
                case Key::emKeyState::KeyState_Pressed:{
                    
                }break;

                case Key::emKeyState::KeyState_Released:{
                    
                }break;

                default:{
                    //Serial.print("Powerkey statechange");
                }
            }
        }
        state = KeyOneCup.UpdateKeyState(pins->GetPinStatus(pinname::SWITCH3));
        if(true == state.has_changed){
            /* Emitt new key state as event */
            emittkeyevent(Key::emMachineKeys::OneCup, state.state );
            switch(state.state ){
                case Key::emKeyState::KeyState_Pressed:{
                    
                }break;

                case Key::emKeyState::KeyState_Released:{
                    
                }break;

                default:{
                    
                }
            }
        }

        uint32_t ulNotificationValue=0;
        ulNotificationValue = ulTaskNotifyTake( true, 0); /* we won't block here ....*/
        if(0 != ulNotificationValue){
            /* we need to process an injected event */
            for(uint32_t i=0; i<( sizeof(eventinputentrytable)/sizeof(eventinputentrytable[0]) );i++){
                if(NULL != eventinputentrytable[i].handle){
                    if( ((1<<eventinputentrytable[i].bit) & ulNotificationValue ) != 0){
                        emittkeyevent(eventinputentrytable[i].machinekey, eventinputentrytable[i].event);
                    }
                }
            }
        }

        // state = KeyDisplay.UpdateKeyState(pins->GetPinStatus(pinname::DISP_BTN));  
        // if(true == state.has_changed){
        //     /* Emitt new key state as event */
        //     emittkeyevent(Key::emMachineKeys::Display, state.state );
        // }
        digitalWrite(12,LOW);
        delay(100); /* 100ms delay, no cpu time will be burned */

    }
    return;
}


void StartKeyTask( SenseEHajoGPIOPins* pins ){
    TaskParams params ;
    params.pins = pins;
    /* from the docs:  https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#background-tasks */
    /*
        IDLE Task = Prio 0
        Main Task = Prio 1
        Key Task = Prio 2
        IPC Task = Prio 24
        ESP TIMER TASK = Prio 22
    */
    /*
        To get a more or less cyclic cpu call / timeslot we need to start with prio 2
        to be able to interrupt the main loop() function
    */

    bzero(&eventinputentrytable[0],sizeof(eventinputentrytable));
    bzero(&evententrytable[0],sizeof(evententrytable));

    if(pdPASS  != xTaskCreate( KeyTask, "KeyTask", 4096, (void*)&params, tskIDLE_PRIORITY+2, &KeyTaskxHandle )){
        /* Fail */
    } else {
        /* Task up and running */
        configASSERT( KeyTaskxHandle );
    }

}

