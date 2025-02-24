#ifndef __JSONINTERFACE_H__
    #define __JSONINTERFACE_H__
    #include <Arduino.h>
    typedef struct {
        uint32_t cupsizeMilliLiter; /* default value if no cup is defined */
        float coffeetemp;
        char cupcfgfile[255];
    } coffecup_config_t;

    typedef struct {
        float timeToMilliLiter;
    } pump_config_t;

    typedef struct {
        float standbytemp;
        uint32_t idletime;        
    } machine_config_t;

    coffecup_config_t readCoffeeCupConfig(void);
    void writeCoffeeCupConfig(coffecup_config_t config);

    pump_config_t readPumpConfig(void);
    void writePumpConfig(pump_config_t config);
    
    machine_config_t readMachineConfig(void);
    void writeMachineConfig(machine_config_t config);


#endif