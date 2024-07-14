
#pragma once
#include <Arduino.h>

class Key {
    public:
        enum emKeyState { KeyState_Released, KeyState_Pressed, KeyState_LongPressed, KeyState_LongReleased};
        enum emMachineKeys { OneCup, TwoCups, Power, Display};
        Key() = delete;
        Key(bool isInverted);
        struct keystate{
          emKeyState state;
          bool has_changed;
        };

        [[nodiscard]] emKeyState GetKeyState();
        keystate UpdateKeyState(bool pysicalstate);

      private:
        bool _isInverted;
        int32_t _debounceval;
        uint32_t _pressduration;
        uint32_t _releaseduration;
        emKeyState _internalstate;
        const int32_t _debouncelimithigh = 3;
        const int32_t _debouncelimitlow = -3;

};