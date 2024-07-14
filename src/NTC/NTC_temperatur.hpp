#include "stdint.h"

class NTC10K{
    public:
        NTC10K() = delete;
        NTC10K(float ref_resistor_val);
        enum tempunit { Kelvin=0,Celsius, Fahrenheit};
        float GetTemp(uint32_t ADCValue, uint32_t ADCFullScale, tempunit unit);
        
        
    private:
        float _resistor_value;
        //Const values for NTC10K
        const float referenceVoltage = 3.3;
        //const float referenceResistor = 10000; // the 'other' resistor
        const float beta = 3950; // Beta value (Typical Value)
        const float nominalTemperature = 25; // Nominal temperature for calculating the temperature coefficient
        const float nominalResistance = 10000; // Resistance value at nominal temperature

};