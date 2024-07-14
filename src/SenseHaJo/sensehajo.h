#include "../MQTT/mqtt_client.h"
#include "../WebService/WebService.hpp"
#include "../LED/led.hpp"
#include "pinout/pinout.hpp"
#ifdef __cplusplus
    extern "C" {
#endif

void sensehajo(led* lights, mqtt_client* mqtt_client,WebService* webservice);

#ifdef __cplusplus
    }
#endif
