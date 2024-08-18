#include "key_task/key_task.hpp"
#include "boilertempreg/boilertempreg.h"
#include "sensehajo.h"



mqtt_client* mqtt_ptr = nullptr;
WebService* webserices_ptr = nullptr;
led* led_ptr= nullptr;        
SenseEHajoGPIOPins pins;
TaskHandle_t xHandle = NULL;

/* 
    for our keys we need some conventions and bitfields set 
    we use the const keyword to force the const into flash
    and also to be readonly for a compiler perspective    
*/


enum emKeyEvent {
    PowerKeyReleased=0,
    OneCupKeyReleased,
    TwoCupsKeyReleased,
    PowerKeyPressed,
    OneCupKeyPressed,
    TwoCupsKeyPressed,
    emKeyEventCNT
};
const uint32_t bv_KeyEventBit[emKeyEventCNT] ={
[PowerKeyReleased] = _BV(PowerKeyReleased),
[OneCupKeyReleased] = _BV(OneCupKeyReleased),
[TwoCupsKeyReleased] = _BV(TwoCupsKeyReleased),
[PowerKeyPressed] = _BV(PowerKeyPressed),
[OneCupKeyPressed] = _BV(OneCupKeyPressed),
[TwoCupsKeyPressed] = _BV(TwoCupsKeyPressed)
};

enum emFSMstate{
    FSM_STATE_IDLE=0,
    FSM_STATE_POWERUP,
    FSM_STATE_STANDBY,
    FSM_STATE_START_BREW,
    FSM_STATE_PREHEAT,
    FSM_STATE_HEAT,
    FSM_STATE_PRE_WATER,
    FSM_STATE_BREW,
    FSM_STATE_ERROR,
    FSM_STATE_COLD_CLEAN,
    FSM_STATE_HOT_CLEAN
};

void start (void);
void task(void* pvParameters);   

void sensehajo(led* lights, mqtt_client* mqtt_client,WebService* webservice){

    /* we need some generic components */
    led_ptr = lights;
    mqtt_ptr = mqtt_client;
    webserices_ptr = webservice;
    start();

}

void start(void){
    pins.Setup();
    StartKeyTask(&pins);
    start_Boilertempreg(&pins,HEATER,ANALOG_IN);
    /* 
        Here we have the same as with any other task, the priority 
        as the main loop() will be prio 1 we need to be at least 
        prio 2 or even 2+ here, as the loop() shall only get the 
        cpu time we don't need in other tasks
    */
    if(pdPASS  != xTaskCreate( &task, "CoffeeFSMTask", 4096, nullptr, tskIDLE_PRIORITY+1, &xHandle ))
    {
        /* Fail */
    } else {
        /* Task up and running */
        configASSERT( xHandle );
    }

    RegisterForKeyEvent(xHandle, Key::emKeyState::KeyState_Released, Key::emMachineKeys::Power,bv_KeyEventBit[PowerKeyReleased]);
    RegisterForKeyEvent(xHandle, Key::emKeyState::KeyState_Released, Key::emMachineKeys::OneCup,bv_KeyEventBit[OneCupKeyReleased]);
    RegisterForKeyEvent(xHandle, Key::emKeyState::KeyState_Released, Key::emMachineKeys::TwoCups,bv_KeyEventBit[TwoCupsKeyReleased]);
    RegisterForKeyEvent(xHandle, Key::emKeyState::KeyState_Pressed,  Key::emMachineKeys::Power,bv_KeyEventBit[PowerKeyPressed]);
    RegisterForKeyEvent(xHandle, Key::emKeyState::KeyState_Pressed,  Key::emMachineKeys::OneCup,bv_KeyEventBit[OneCupKeyPressed]);
    RegisterForKeyEvent(xHandle, Key::emKeyState::KeyState_Pressed,  Key::emMachineKeys::TwoCups,bv_KeyEventBit[TwoCupsKeyPressed]);
}

void task(void* pvParameters){
    
  
    /* Set machine into a default state */
    emFSMstate fsmstate = FSM_STATE_IDLE;
    /* Turn off heater and pump, just to be sure */
    pins.SetPinStatus(pinname::HEATER,false);
    pins.SetPinStatus(pinname::PUMP,false);
    /* Read temperature from ADC*/

    /* Set all LEDs off */
    
    led_ptr->setColor(0,led::color(0,0,0));
    led_ptr->setColor(1,led::color(0,0,0));
    led_ptr->setColor(2,led::color(0,0,0));
    /* 
        using while(1) or while(true) works here, but to be sure 
        that the compiler will never end the loop using a 
        while ( 1==1 ) would be a lot better. This will force a logic
        'true' what ever this means for the compiler. Someone could have a
        #define that may alters true or false to something we don't want.
    */
    while(true){
    /* Handle FSM and Key presses */
    
        uint32_t ulNotificationValue=0;
        ulNotificationValue = ulTaskNotifyTake( true, 0); /* we won't block here ....*/
        if(0 != (ulNotificationValue & bv_KeyEventBit[PowerKeyReleased] )){
            Serial.println("Power Key released");
        }

        if(0 != (ulNotificationValue & bv_KeyEventBit[OneCupKeyReleased] )){
            Serial.println("OneCup Key released");
        }

        if(0 != (ulNotificationValue & bv_KeyEventBit[TwoCupsKeyReleased] )){
            Serial.println("TwoCup Key released");
        }

        if(0 != (ulNotificationValue & bv_KeyEventBit[PowerKeyPressed] )){
            Serial.println("Power Key pressed");
        }

        if(0 != (ulNotificationValue & bv_KeyEventBit[OneCupKeyPressed] )){
            Serial.println("OneCup Key pressed");
        }

        if(0 != (ulNotificationValue & bv_KeyEventBit[TwoCupsKeyPressed] )){
            Serial.println("TwoCup Key pressed");
        }

        switch (fsmstate) {
            case FSM_STATE_IDLE:{
                /*
                    boiler off 
                    led off
                    pump off                     




                */

            }break;

            case FSM_STATE_POWERUP:{
                /*  boiler to pretemp
                    led heat up state
                    pump off
                */
            }break;
            case FSM_STATE_STANDBY:{

            }break;
            case FSM_STATE_START_BREW:{

            }break;
            case FSM_STATE_PREHEAT:{

            }break;
            case FSM_STATE_HEAT:{

            }break;
            case FSM_STATE_PRE_WATER:{

            }break;
            case FSM_STATE_BREW:{

            }break;
            case FSM_STATE_ERROR:{

            }break;

            case FSM_STATE_COLD_CLEAN:{

            } break;
            
            case FSM_STATE_HOT_CLEAN:{

            } break;
        }
       
        


    
    }

}

