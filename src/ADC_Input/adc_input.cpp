#include "adc_input.h"

void ADCInit ( void ){
    analogReadResolution( 12 );
}

uint32_t ADCRead ( uint8_t pin ){

    uint32_t value = 0;
    for( uint32_t i=0;i<16;i++){
        value = value + analogRead(pin);
    }
    value = value / 16 ;
    return value;

}