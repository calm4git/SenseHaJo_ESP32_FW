#include <Arduino.h>
#include "../pinout/pinout.hpp"

enum emPumpErrorEvent{
     PumpNoError=0,
     PumpOutOfWater
};

enum emPumpEvent{
    PumpReady=0,
    PumpingDone,
    PumpingAbort,
    PumpingStart
};

enum emPumpConveyUnit {
    PumpConveyMilliLiter=0,
    PumpConveySeconds
};

void StartPumpTask( SenseEHajoGPIOPins* pins, pinname pumppin, pinname reedwaterempty );

bool RegisterForPumpEvent(xTaskHandle handle, emPumpEvent Event, uint32_t eventmask );
bool UnRegisterForPumpEvent( xTaskHandle handle, emPumpEvent Event);

bool RegisterForPumpErrorEvent(xTaskHandle handle, emPumpErrorEvent Event, uint32_t eventmask );
bool UnRegisterForPumpErrorEvent( xTaskHandle handle, emPumpErrorEvent Event);

bool conveywater( uint32_t amount,emPumpConveyUnit unit );
bool conveyabort( void );
bool conveyinprogress( void );
emPumpErrorEvent hasError(void);