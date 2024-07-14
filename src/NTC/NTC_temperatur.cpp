#include <math.h>
#include "NTC_temperatur.hpp"

NTC10K::NTC10K(float ref_resistor_val){
    _resistor_value=ref_resistor_val;
}

float NTC10K::GetTemp(uint32_t ADCValue, uint32_t ADCFullScale, tempunit unit){
    float temp=NAN;
    float voltage = (ADCValue * referenceVoltage) / (float)ADCFullScale; // Calculate voltage
    float resistance = (voltage * _resistor_value) / (referenceVoltage - voltage); // Calculate thermistor resistance with updated configuration

    // Calculate temperature using the Beta parameter equation
    //float tempK = 1 / (((log(resistance / nominalResistance)) / beta) + (1 / (nominalTemperature + 273.15)));
    //float tempC = tempK - 273.15; // Get temperature in Celsius
    //float tempF = 1.8 * tempC + 32.0; // Get temperature in Fahrenheit
    switch(unit){
        case Kelvin:{
            temp= 1 / (((log(resistance / nominalResistance)) / beta) + (1 / (nominalTemperature + 273.15)));
        } break;

        case Celsius:{
            temp= (1 / (((log(resistance / nominalResistance)) / beta) + (1 / (nominalTemperature + 273.15))))-273.15;           
        } break;

        case Fahrenheit:{
            temp= (1.8*((1 / (((log(resistance / nominalResistance)) / beta) + (1 / (nominalTemperature + 273.15))))-273.15))+32;           
        } break;
    }
    return temp;  
}