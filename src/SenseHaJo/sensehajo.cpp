#include "FS.h"
#include <LittleFS.h>

#include "key_task/key_task.hpp"
#include "boilertempreg/boilertempreg.h"
#include "pumptask/pumptask.h"
#include "sensehajo.h"
#include "cup/cup.h"
#include "jsoninterface/jsoninterface.h"



mqtt_client* mqtt_ptr = nullptr;
WebService* webserices_ptr = nullptr;
led* led_ptr= nullptr;        
SenseEHajoGPIOPins pins;
TaskHandle_t xHandle = NULL;
BoilerTempQHandle_t BoilerTemperaturQ;

coffeeCup cup;

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
    FSM_STATE_PRE_WATER_WAIT,
    FSM_STATE_BREW,
    FSM_STATE_BREW_FINISH,
    FSM_STATE_ERROR,
    FSM_STATE_COLD_CLEAN,
    FSM_STATE_COLD_CLEAN_START,
    FSM_STATE_COLD_CLEAN_END,
    FSM_STATE_HOT_CLEAN,
    FSM_STATE_HOT_CLEAN_START,
    FSM_STATE_HOT_CLEAN_END

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
    Serial.println("Start senseHaJo");
    pins.Setup();
    StartKeyTask(&pins);
    BoilerTemperaturQ =  xRingbufferCreate((sizeof(tempdata_t)+8)*2, RINGBUF_TYPE_NOSPLIT);
    BoilertempRegisterDataQueue(BoilerTemperaturQ, sizeof(tempdata_t));

    StartPumpTask(&pins,PUMP,REED);
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
    Serial.println("Start task");
    uint32_t idlecounter=0; 
    uint32_t preWaterWait=0;
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
    
    /* Read configs */
    coffecup_config_t cupconfig = readCoffeeCupConfig();
    machine_config_t machineconfig = readMachineConfig();



    cup.setVolume(cupconfig.cupsizeMilliLiter); /* get size from machine config */
    if(false == cup.loadSettings(cupconfig.cupcfgfile)){
        Serial.print("failed to load default cup");
    }
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

        size_t datasize=0;
        tempdata_t* data = NULL;
        data =(tempdata_t*)(xRingbufferReceive(BoilerTemperaturQ, &datasize, 0));
        if(datasize == sizeof(tempdata_t)){
            /* we have new tempdata */
            //Serial.printf("FSM Temp Value: %f\r\n", data->value);
            webserices_ptr->WebSocketUpdateBoilertemp(data->value);
            vRingbufferReturnItem(BoilerTemperaturQ,data);
        }
        switch (fsmstate) {
            case FSM_STATE_IDLE:{
                /*
                    boiler off 
                    led off
                    pump off                     
                */
                setBoilerTargetTemp(0);
                conveyabort();
                led_ptr->setColor(0,led::color(0,0,0));
                led_ptr->setColor(1,led::color(0,0,0));
                led_ptr->setColor(2,led::color(0,0,0));
                if(0 != ulNotificationValue){
                    if(0 != (ulNotificationValue & bv_KeyEventBit[PowerKeyReleased] )){
                        
                        fsmstate = FSM_STATE_POWERUP;

                    } else if(0 != (ulNotificationValue & bv_KeyEventBit[TwoCupsKeyReleased] )){

                        if(hasError()==false){
                            fsmstate = FSM_STATE_START_BREW;
                        }
                    
                    } else if(0 != (ulNotificationValue & bv_KeyEventBit[OneCupKeyReleased] )){

                        if(hasError()==false){
                            fsmstate = FSM_STATE_START_BREW;    
                        }
                    }

                    if((pins.GetPinStatus(pinname::SWITCH1)==true) && (pins.GetPinStatus(pinname::SWITCH3)==true)){
                        if(hasError()==false){
                            fsmstate=FSM_STATE_COLD_CLEAN;
                        }
                    }

                }
            }break;

            case FSM_STATE_POWERUP:{
                /*  
                    boiler to pretemp
                    led heat up state
                    pump off
                */
                setBoilerTargetTemp(machineconfig.standbytemp);
                conveyabort();

                led_ptr->setColor(0,led::color(0,0,0));
                led_ptr->setEffect(0,led::led_effect_t::off);

                led_ptr->setColor(1,led::color(255,0,0));
                led_ptr->setEffect(1,led::led_effect_t::blink_med);
                
                led_ptr->setColor(2,led::color(0,0,0));
                led_ptr->setEffect(2,led::led_effect_t::off);

                if( ( getBoilerCurrentTemp() > (60-2) ) && ( getBoilerCurrentTemp() < (60 + 2) ) ){
                    fsmstate = FSM_STATE_STANDBY;
                } else {
                    if(0 != (ulNotificationValue & bv_KeyEventBit[PowerKeyReleased] )){
                        
                        fsmstate = FSM_STATE_IDLE;
                        idlecounter=0;

                    } else if(0 != (ulNotificationValue & bv_KeyEventBit[TwoCupsKeyReleased] )){
                        cup.setDoubleVolume(true);
                        if(hasError()==false){
                            fsmstate = FSM_STATE_START_BREW;
                        }
                    
                    } else if(0 != (ulNotificationValue & bv_KeyEventBit[OneCupKeyReleased] )){
                        cup.setDoubleVolume(false);
                        if(hasError()==false){
                            fsmstate = FSM_STATE_START_BREW;    
                        }
                    }
                }
                
            }break;
            case FSM_STATE_STANDBY:{
                setBoilerTargetTemp(machineconfig.standbytemp);
                conveyabort();
                
                led_ptr->setColor(0,led::color(0,0,0));
                led_ptr->setEffect(0,led::led_effect_t::off);

                led_ptr->setColor(1,led::color(0,0,255)); /* blue */
                led_ptr->setEffect(1,led::led_effect_t::on);
                
                led_ptr->setColor(2,led::color(0,0,0));
                led_ptr->setEffect(2,led::led_effect_t::off);

                if(0 != (ulNotificationValue & bv_KeyEventBit[PowerKeyReleased] )){
                        
                    fsmstate = FSM_STATE_IDLE;

                } else if(0 != (ulNotificationValue & bv_KeyEventBit[TwoCupsKeyReleased] )){
                    cup.setDoubleVolume(true);
                    if(hasError()==false){
                        fsmstate = FSM_STATE_START_BREW;
                    }
                
                } else if(0 != (ulNotificationValue & bv_KeyEventBit[OneCupKeyReleased] )){
                    cup.setDoubleVolume(false);
                    if(hasError()==false){
                        fsmstate = FSM_STATE_START_BREW;    
                    }
                } else {
                    
                    

                    if(idlecounter<UINT32_MAX){
                        idlecounter++;
                    }

                    if(idlecounter>10*60*30){
                      fsmstate = FSM_STATE_IDLE;
                      idlecounter=0; 
                    }

                    if((pins.GetPinStatus(pinname::SWITCH1)==true) && (pins.GetPinStatus(pinname::SWITCH3)==true)){
                        if(hasError()==false){
                            fsmstate=FSM_STATE_HOT_CLEAN;
                        }
                    }
                    
                }
            }break;

            case FSM_STATE_START_BREW:{
                /* Set LEDs, check if lid is closed, check if 1 or 2 cups */
                fsmstate = FSM_STATE_PREHEAT;
            }break;

            case FSM_STATE_PREHEAT:{
                    setBoilerTargetTemp(machineconfig.standbytemp);
                    if( ( getBoilerCurrentTemp() > (60-2) ) && ( getBoilerCurrentTemp() < (60 + 2) ) ){
                        fsmstate = FSM_STATE_HEAT;
                    }
            }break;

            case FSM_STATE_HEAT:{
                    setBoilerTargetTemp(cupconfig.coffeetemp);
                    if( ( getBoilerCurrentTemp() > (cupconfig.coffeetemp-2) ) && ( getBoilerCurrentTemp() < (cupconfig.coffeetemp + 2) ) ){
                        fsmstate = FSM_STATE_PRE_WATER;
                        if(false == conveyinprogress()){
                            if(hasError()==false){
                                conveywater(2,emPumpConveyUnit::PumpConveySeconds);
                            } else {
                            fsmstate=FSM_STATE_ERROR;
                            }
                        } else {
                            conveyabort();
                            fsmstate=FSM_STATE_ERROR;
                        }
                    }
            }break;

            case FSM_STATE_PRE_WATER:{
                if(false == conveyinprogress() ){
                    if(hasError()==false){
                        fsmstate==FSM_STATE_PRE_WATER_WAIT;
                        preWaterWait=0;
                    } else {
                        fsmstate=FSM_STATE_ERROR;
                    }
                } else {
                    /* stay */
                }
                                       
            }break;

            case FSM_STATE_PRE_WATER_WAIT:{
                /* we need to wait 5 seconds */
                        
                if(preWaterWait<10*5){
                    if(preWaterWait<UINT32_MAX){
                        preWaterWait++;
                    }
                } else {
                    fsmstate=FSM_STATE_BREW;
                }
            }

            case FSM_STATE_BREW:{
                /* Start pump and raise temp */
                if( (conveyinprogress()==false)&&(hasError()==false)){

                    conveywater(cup.getVolume(),emPumpConveyUnit::PumpConveyMilliLiter); /* we grab that from our cup settings */
                    fsmstate=FSM_STATE_BREW_FINISH;
                } else {
                     fsmstate=FSM_STATE_ERROR;
                }
            }break;

            case FSM_STATE_BREW_FINISH:{
                if( (conveyinprogress()==false)&&(hasError()==false)){
                    /* coffee is done */
                    fsmstate=FSM_STATE_STANDBY;
                } else {
                    if(hasError()==true){
                        fsmstate=FSM_STATE_ERROR;
                    } 
                }

            } break;

            case FSM_STATE_ERROR:{
                /* Let the user press a button - quit with power button */
                if(0 != (ulNotificationValue & bv_KeyEventBit[PowerKeyReleased] )){
                    fsmstate = FSM_STATE_STANDBY;
                }
            }break;

            case FSM_STATE_COLD_CLEAN:{
                if( (conveyinprogress()==false)&&(hasError()==false)){
                    conveywater(1000,emPumpConveyUnit::PumpConveyMilliLiter);
                    fsmstate = FSM_STATE_COLD_CLEAN_START; 
                }
            } break;

            case FSM_STATE_COLD_CLEAN_START:{
                if( (conveyinprogress()==false)&&(hasError()==true)){
                    fsmstate = FSM_STATE_COLD_CLEAN_END;  
                } else {

                }
            } break;
            
            case FSM_STATE_COLD_CLEAN_END:{
                fsmstate = FSM_STATE_IDLE;  
            } break;

            case FSM_STATE_HOT_CLEAN:{
                setBoilerTargetTemp(90); /* hot clean config temp needs to be moved into config */
                if( (getBoilerCurrentTemp()>90-2) ){
                    if( (conveyinprogress()==false)&&(hasError()==false)){
                        conveywater(1000,emPumpConveyUnit::PumpConveyMilliLiter);
                        fsmstate = FSM_STATE_HOT_CLEAN_START; 
                    }
                }
                
            } break;

            case FSM_STATE_HOT_CLEAN_START:{
                 if( (conveyinprogress()==false)&&(hasError()==true)){
                    fsmstate = FSM_STATE_HOT_CLEAN_END;  
                } else {

                }
            } break;

            case FSM_STATE_HOT_CLEAN_END:{
                fsmstate = FSM_STATE_POWERUP;            
            } break;
        }
       
        delay(100); /* 100ms sleep , we give back CPU time => 10Hz */


    
    }

}

void LoadSettings(void){
    /* this loads some settings from littlfs */

    /* we use a JSON file stored in root as settings.json */

    /* if the file is non existant we will create one with default settings */
    fs::FS fs = LittleFS;
    
    File file = fs.open("settings.json");
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        /* Create new file */

    } else {
        Serial.println("- read from file:");
        while(file.available()){
            Serial.write(file.read());
        }
        file.close();
    }

    

}