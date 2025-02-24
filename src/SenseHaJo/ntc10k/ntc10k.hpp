#include "cmath"


class NTC10K {
public:
    // Constructor
    NTC10K(float seriesResistor, float nominalResistance, float nominalTemperature, float bCoefficient, int adcMaxValue)
        : seriesResistor(seriesResistor), nominalResistance(nominalResistance), nominalTemperature(nominalTemperature),
          bCoefficient(bCoefficient), adcMaxValue(adcMaxValue) {}

    // Method to convert ADC value to temperature
    float convertADCToTemperature(int adcValue) {
        float resistance = calculateResistance(adcValue);
        return calculateTemperature(resistance);
    }

private:
    float seriesResistor;
    float nominalResistance;
    float nominalTemperature;
    float bCoefficient;
    int adcMaxValue;

    // Method to calculate resistance from ADC value
    float calculateResistance(int adcValue) {
        if(adcValue<1){
            adcValue=1; //avoid division by zero
        }
        /*
        float voltageDivider = (float)adcValue / adcMaxValue;
        float calcResistance = seriesResistor * (1.0 / voltageDivider - 1.0);
        */
        /*
                    ------- 3V3
               |    |
               Ir  ---
               |  | R | <--3k3
               v   --- 
                    |
                    ----- Vin
                    |
               |   ---
               Ir |NTC|
               |   --- 
               V    |
                    ---- GND

            R_Vdrop = 3,3 - Vin
            Ir = R_Vdrop / 3300R
            NTC_Vdrop = Vin
            R_NTC = NTC_Vdrop / Ir
          
            Vin = 3.3/adcmax*adcValue
        */
        /*Problem with the ADC is that we have an offset of 0.1V to 0.15V */
        float Vin = ( (float)3.5* ((float)adcValue / (float)adcMaxValue ) );
        /* we need to limit Vin to 3.3 Volt and even that would be an error */
        if(Vin>=3.3){
            Vin=3.3; 
        }
        float R_vdrop=3.3-Vin;
        /* If R_vdrop is 3.3V it measn we have a short in the NTC */
        if( R_vdrop <= 0.0){
            R_vdrop=0.001;
        }
        /* We need to increase R_vdrop to someting a little above zero */
        float Ir=R_vdrop / seriesResistor;
        /* We will get something very small as Ir if R_vdrop gets small */
        float NTC_vdrop = Vin; //This is basically 3.3V-R_vdrop=Vin
        /* we have the current and voltage drop over the ntc*/
        float Rntc= NTC_vdrop/Ir;
        /* This means we can also write the formula i less steps */
        // float Rntc = Vin / ( ( 3.3-Vin ) / seriesResistor );  
        return Rntc;
    }

    // Method to calculate temperature from resistance using Steinhart-Hart equation
    float calculateTemperature(float resistance) {
        float steinhart;
        steinhart = resistance / nominalResistance;             // (R/Ro)
        steinhart = log(steinhart);                             // ln(R/Ro)
        steinhart /= bCoefficient;                              // 1/B * ln(R/Ro)
        steinhart += 1.0 / (nominalTemperature + 273.15);       // + (1/To)
        steinhart = 1.0 / steinhart;                            // Invert
        steinhart -= 273.15;                                    // Convert to Celsius
        return steinhart;
    }
};