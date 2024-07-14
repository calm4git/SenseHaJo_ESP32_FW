#ifndef __key_task_hpp__
 #define __key_task_hpp__

#include <Arduino.h>
#include "../pinout/pinout.hpp"
#include "key_class.hpp"


void StartKeyTask(SenseEHajoGPIOPins* pins);
bool RegisterForKeyEvent(xTaskHandle handle, Key::emKeyState Event, Key::emMachineKeys Key, uint32_t eventmask );
bool UnRegisterForKeyEvent( xTaskHandle handle, Key::emKeyState Event, Key::emMachineKeys Key);

#endif


