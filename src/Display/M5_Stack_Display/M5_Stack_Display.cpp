#include "../Display.h"

#ifdef M5STACK_LCD
  //Init TFT Display with TFT_eSPI 
  #include "M5_Stack_Display.h"
#endif

#ifdef TFT_eSPI_DISPLAY
      TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
#endif

void M5_Stack_Display_Init(void){
  #ifdef TFT_eSPI_DISPLAY
      #ifdef M5STACK_LCD
        //Init TFT Display with TFT_eSPI
        tft.init();
        tft.invertDisplay( true );
        tft.setRotation(1);
        tft.fillScreen(TFT_BLACK);     
      #endif
  #endif
}
#ifdef TFT_eSPI_DISPLAY
  TFT_eSPI* M5_Stack_GetDisplay(void){
    return &tft;
  }
#endif