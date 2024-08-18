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
        float voltageDivider = (float)adcValue / adcMaxValue;
        return seriesResistor * (1.0 / voltageDivider - 1.0);
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