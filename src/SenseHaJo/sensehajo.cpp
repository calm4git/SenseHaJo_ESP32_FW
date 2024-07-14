#include "key_task/key_task.hpp"

#include "sensehajo.h"

mqtt_client* mqtt_ptr = nullptr;
WebService* webserices_ptr = nullptr;
led* led_ptr= nullptr;        
SenseEHajoGPIOPins pins;
TaskHandle_t xHandle = NULL;

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
    if(pdPASS  != xTaskCreate( &task, "CoffeeFSMTask", 4096, nullptr, tskIDLE_PRIORITY+1, &xHandle ))
    {
        /* Fail */
    } else {
        /* Task up and running */
        configASSERT( xHandle );
    }

    RegisterForKeyEvent(xHandle, Key::emKeyState::KeyState_Released, Key::emMachineKeys::Power,0x01);
    RegisterForKeyEvent(xHandle, Key::emKeyState::KeyState_Released, Key::emMachineKeys::OneCup,0x02);
    RegisterForKeyEvent(xHandle, Key::emKeyState::KeyState_Released, Key::emMachineKeys::TwoCups,0x04);
}

void task(void* pvParameters){
    /* Set machine into a default state */

    /* Turn off heater and pump, just to be sure */

    /* Read temperature from ADC*/

    /* Set all LEDs off */

    
    while(true){
    /* Handle FSM and Key presses */
    
        uint32_t ulNotificationValue=0;
        ulNotificationValue = ulTaskNotifyTake( true, 0); /* we won't block here ....*/
        delay(100);
        /* Define FSM here */


    
    }

}

