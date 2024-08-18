#ifndef __led_h__
 #define __led_h__
    #include <stdint.h>
    class led{
        public:
            typedef enum {
                off=0,
                on,
                blink_slow,
                blink_med,
                blink_fast,
                color_change,
                rainbow
            } led_effect_t;

            
            struct color{
                struct rgbw_t{
                    uint8_t b; /* blue  */
                    uint8_t r; /* red   */
                    uint8_t g; /* green */
                    uint8_t w; /* white */
                };

                union  color_t{
                    uint32_t _u32;
                    rgbw_t _rgbw;
                };

                color_t val;

                color(){
                    val._u32=0;
                }

                color(uint8_t r, uint8_t g, uint8_t b){
                    val._rgbw.r=r;
                    val._rgbw.b=b;
                    val._rgbw.g=g;
                    val._rgbw.w=0;
                }

                color(uint32_t value){
                    val._u32=value;
                }

                color_t getColor_t(){
                    return val;
                }

                uint32_t getColor(){
                    return val._u32;
                }

                rgbw_t getRGBW(){
                    return val._rgbw;
                }

                uint8_t getR(){
                    return val._rgbw.r;
                }

                uint8_t getG(){
                    return val._rgbw.g;
                }
                
                uint8_t getB(){
                    return val._rgbw.b;
                }
                
                uint8_t getW(){
                    return val._rgbw.w;
                }

                void setColor(color_t value){
                    val=value;
                }

                void setColor(rgbw_t rgbw){
                    val._rgbw=rgbw;
                }

                void setColor(uint32_t value){
                    val._u32=value;
                }


            } ;
            

            led();
            ~led();  
            void setEffect(uint32_t led, led_effect_t effect);
            void setColor(uint32_t led,color color );
            void setBrightness(uint32_t led, uint8_t bright);
            void init(void);
            void loop(void);
        private:

    };
#endif
