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

            
            typedef union{
                    uint32_t value;
                    struct {
                        uint8_t b; /* blue  */
                        uint8_t r; /* red   */
                        uint8_t g; /* green */
                        uint8_t w; /* white */
                    } rgbw;
            } color_t;
            

            led();
            ~led();  
            void setEffect(uint32_t led, led_effect_t effect);
            void setColor(uint32_t led,color_t color );
            void setBrightness(uint32_t led, uint8_t bright);
            void init(void);
            void loop(void);
        private:

    };
#endif
