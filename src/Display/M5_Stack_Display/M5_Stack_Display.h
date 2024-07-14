#ifdef TFT_eSPI_DISPLAY
    #ifdef M5STACK_LCD
    
    #include "TFT_eSPI.h"
    
    void M5_Stack_Display_Init(void);
    TFT_eSPI* M5_Stack_GetDisplay(void);

    #endif
#endif