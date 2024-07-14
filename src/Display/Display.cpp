#include "Display.h"
#ifdef TFT_eSPI_DISPLAY
    uint32_t colors[]={
        TFT_BLACK,
        TFT_RED,
        TFT_BLUE,
        TFT_GREEN
    };


    #ifdef M5STACK_LCD
        #include "M5_Stack_Display/M5_Stack_Display.h"
    #endif

    void DisplayInit(void){
        #ifdef M5STACK_LCD
            M5_Stack_Display_Init();
        #endif
    }

    TFT_eSPI* DisplayGetPtr(void){
        #ifdef M5STACK_LCD
            return M5_Stack_GetDisplay();
        #else
            return NULL;
        #endif
    }

    void DisplayLoop(void){
        static uint32_t lastrun=millis();
        static uint8_t index=0;
        if( (millis()-lastrun)>1000){
            lastrun=millis();
            TFT_eSPI* display = DisplayGetPtr();
            if(NULL != display){
                display->fillScreen(colors[index]);     
                index++;
                if ( index >= (sizeof(colors)/sizeof(colors[0]) ) ){
                    index=0;
                }
            }
        }        
    }
#endif