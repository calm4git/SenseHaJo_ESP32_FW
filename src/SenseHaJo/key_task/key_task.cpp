#include "Esp.h"
//#include "../pinout/pinout.hpp"
#include "key_task.hpp"
#include "key_class.hpp"




struct TaskParams {
    SenseEHajoGPIOPins* pins;
};

/*  We need a class of key objects that will handle the Key logic 
    as we have four(4) pins we are interrested in we will add the
    objects for these
*/ 

Key KeyOneCup(false); /* OneCup Key*/
Key KeyTwoCups(false); /* TwoCups Key*/
/* If you know, you know ...*/
Key KeyPower(false);
Key KeyDisplay(false);

/* We need to limit the amount of handles that can register */
/* Limit to 32 entrys for the keys */
typedef struct  {
    xTaskHandle handle;
    Key::emKeyState event;
    uint32_t mask;
    Key::emMachineKeys machinekey;
} evententry_t;

evententry_t evententrytable[32];


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
            (evententrytable[i].event = state)     &&
            (evententrytable[i].handle != nullptr)
            ){
            //Send task notify
            xTaskNotify(evententrytable[i].handle,evententrytable[i].mask,eSetBits);
        }
    }
}

void KeyTask(void * pvParameters){
    /* grab pins*/
    pinMode(25,OUTPUT);
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
        digitalWrite(25,HIGH);
        Key::keystate state = KeyTwoCups.UpdateKeyState(pins->GetPinStatus(pinname::SWITCH1));
        if(true == state.has_changed){
            /* Emitt new key state as event */
            emittkeyevent(Key::emMachineKeys::TwoCups, state.state );
            switch(state.state ){
                case Key::emKeyState::KeyState_Pressed:{
                    Serial.print("Two Cups pressed");
                }break;

                case Key::emKeyState::KeyState_Released:{
                    Serial.print("Two Cups released");
                }break;

                default:{
                    Serial.print("Two Cups statechange");
                }
            }
            
        }
        
        state = KeyPower.UpdateKeyState(pins->GetPinStatus(pinname::SWITCH2));
        if(true == state.has_changed){
            /* Emitt new key state as event */
            emittkeyevent(Key::emMachineKeys::Power, state.state );
            switch(state.state ){
                case Key::emKeyState::KeyState_Pressed:{
                    Serial.print("Powerkey pressed");
                }break;

                case Key::emKeyState::KeyState_Released:{
                    Serial.print("Powerkey released");
                }break;

                default:{
                    Serial.print("Powerkey statechange");
                }
            }
        }
        state = KeyOneCup.UpdateKeyState(pins->GetPinStatus(pinname::SWITCH3));
        if(true == state.has_changed){
            /* Emitt new key state as event */
            emittkeyevent(Key::emMachineKeys::OneCup, state.state );
            switch(state.state ){
                case Key::emKeyState::KeyState_Pressed:{
                    Serial.print("One cup pressed");
                }break;

                case Key::emKeyState::KeyState_Released:{
                    Serial.print("One cup released");
                }break;

                default:{
                    Serial.print("One cup statechange");
                }
            }
        }
        // state = KeyDisplay.UpdateKeyState(pins->GetPinStatus(pinname::DISP_BTN));  
        // if(true == state.has_changed){
        //     /* Emitt new key state as event */
        //     emittkeyevent(Key::emMachineKeys::Display, state.state );
        // }
        digitalWrite(25,LOW);
        delay(100); /* 100ms delay, no cpu time will be burned */

    }
    return;
}


void StartKeyTask( SenseEHajoGPIOPins* pins ){
    TaskHandle_t xHandle = NULL;
    TaskParams params ;
    params.pins = pins;
    if(pdPASS  != xTaskCreate( KeyTask, "KeyTask", 4096, (void*)&params, tskIDLE_PRIORITY+1, &xHandle )){
        /* Fail */
    } else {
        /* Task up and running */
        configASSERT( xHandle );
    }

}

