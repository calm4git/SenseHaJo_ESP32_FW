#include "led.hpp"
#include <WS2812FX.h>
WS2812FX ws2812fx = WS2812FX(NUM_LEDS, LED_PIN , NEO_RGB + NEO_KHZ800);
led::color pixelcolor[NUM_LEDS];
uint8_t brightness[NUM_LEDS];

led::led(){
    for(uint32_t i=0;i<NUM_LEDS;i++){
        brightness[i]=255;
        pixelcolor[i].setColor(0x00FFFFFF);
    }
    
}

led::~led(){

}

void led::setEffect(uint32_t led, led_effect_t effect){

}

void led::setColor(uint32_t led, color color ){
    if(led<NUM_LEDS){
        pixelcolor[led]=color;
        uint16_t r,g,b;
        r=color.getR()*(255-(255-brightness[led]));
        g=color.getG()*(255-(255-brightness[led]));
        b=color.getB()*(255-(255-brightness[led]));
        r=r/255;
        g=g/255;
        b=b/255;
        ws2812fx.setPixelColor(led,r,g,b);
    }
}

void led::setBrightness(uint32_t led, uint8_t bright){
    if(led<NUM_LEDS){
        /* wrgb color, */
        brightness[led]=bright;
        setColor(led, pixelcolor[led]);
    }
}

void led::init(void){
    if( (NUM_LEDS>0) && (LED_PIN>=0)){
        ws2812fx.init();
        ws2812fx.setBrightness(255);
        ws2812fx.setSpeed(1000);
        ws2812fx.setSegment(0,0,0); /* single led segment */
        ws2812fx.setSegment(1,1,1); /* single led segment */
        ws2812fx.setSegment(2,2,2); /* single led segment */
        ws2812fx.setColor(0,pixelcolor[0].getColor());
        ws2812fx.setColor(1,pixelcolor[1].getColor());
        ws2812fx.setColor(2,pixelcolor[2].getColor());
        ws2812fx.setMode(FX_MODE_STATIC);
        ws2812fx.start();   
    }
}

void led::loop(void){
    if( (NUM_LEDS>0) && (LED_PIN>=0)){
        ws2812fx.service();
    }
}
