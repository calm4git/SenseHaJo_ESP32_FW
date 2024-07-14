
#ifndef __pinout_hpp__
 #define __pinout_hpp__

    #include <stdio.h>
    #include <vector>

// /* Input */
// #define SWITCH1 0
// #define SWITCH2 26
// #define SWITCH3 27
// #define DISP_BTN 18

// /* UART */
// #define MCU_TX 1
// #define MCU_RX 3

// /* LEDs */
// #define WS2812_IO 4

// /* Basic coffee maker  */
// #define PUMP 5
// #define HEATER 17
// #define REED 14

// /* I2C DISPL */
// #define I2C_DISP_SDA 22
// #define I2C_DISP_SCL 23

// /* I2C ADC / EXP */
// #define I2C_SDA 12
// #define I2C_SCL 13

// /* Analog IN / TEMP */
// #define ANALOG_IN 25

// /* Powermeter */
// #define BL0937_CF 34
// #define BL0937_CF1 35
// #define BL0937_SEL 32
// #define MAINS_HALFWAVE 16

#pragma message("TODO: Insert comments for code here !!!")
enum pinname { 
    SWITCH1=0 ,
    SWITCH2,
    SWITCH3,
    /*
    DISP_BTN,
    */
    MCU_RX,
    MCU_TX,
    WS2812_IO,
    PUMP,
    HEATER,
    REED,
    REED2,
    HALSENSOR,
    I2C_DISP_SCL,
    I2C_DISP_SDA,
    ANALOG_IN,
    /*
    BL0937_CF1,
    BL0937_CF,
    BL0937_SEL,
    MAINS_HALFWAVE,
    */
    PINNAME_CNT
};

struct pinconfig_t{
    int32_t pin; //Singed as -1 will mean NOT CONNECTED / NON EXISTENT
} ;

class SenseEHajoAnalogIn{
    public:
        enum emResolution {_9Bits, _10Bits, _11Bits, _12Bits };
        enum emResultStatus { Okay, OutOfRange, NotAnalogPin,Error};
        struct analogvalue{ 
            emResultStatus status; 
            uint16_t value;
        };

        SenseEHajoAnalogIn();
        ~SenseEHajoAnalogIn( ) = default;
        void Setup(const std::vector<pinname>* pinsptr, emResolution resolution=_12Bits);
        analogvalue ReadAnalogIn(pinname pin);
       
    private:
    const std::vector<pinname>* _analogpinptr;

};

class SenseEHajoGPIOPins{
    public:
        SenseEHajoGPIOPins( );
        ~SenseEHajoGPIOPins( );
        void Setup();
        bool GetPinStatus(pinname pin);
        void SetPinStatus(const pinname pin, const bool state);
        SenseEHajoAnalogIn analogin;
    private:
        std::vector<pinname> analogpins;
        std::vector<pinname> digitalin;
        std::vector<pinname> digitalout;
        const std::vector<pinname>* GetAnalogPins(void);
        
};



#endif
