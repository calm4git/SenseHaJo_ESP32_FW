#include <Arduino.h>

class coffeeCup{
    public:
    
        coffeeCup(void);
        ~coffeeCup(void);

        bool loadSettings(String filename); /* We save data to LittleFS */
        bool saveSettings(String filename);
        String GetLoadedFilename(void);
        uint32_t getVolume(void);
        void setVolume(uint32_t volumeMilliLiter);
        /* Non persiten settings */
        bool isDoubleVolume(void);
        void setDoubleVolume(bool doubleVolume);

    private:

        String _filename;
        uint32_t _volume;
        bool _isDouble;
    
};