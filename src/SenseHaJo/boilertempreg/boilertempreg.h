#if __has_include("message_buffer.h") 
    #define BoilerTempQHandle_t MessageBufferHandle_t 
#else
    #if __has_include("freertos/ringbuf.h") 
    #include "freertos/ringbuf.h"
        #define BoilerTempQHandle_t RingbufHandle_t  
    #else
        #error "No MessageBuffer / RingBuffer defined "
    #endif
#endif
//#include "message_buffer.h" <- not supported by ESP32 IDF see https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/freertos_additions.html
#include "../pinout/pinout.hpp"
typedef struct{
    float value;
} tempdata_t;



void start_Boilertempreg(SenseEHajoGPIOPins* pptr, pinname p, pinname adcp);
float getBoilerTargetTemp();
void setBoilerTargetTemp(float temp);
float getBoilerCurrentTemp(void);
void Boilertempreg_task(void* param);
bool registerDataQueue(BoilerTempQHandle_t qhandle, uint32_t elementsize);