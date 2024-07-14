#include "key_class.hpp"

Key::Key(bool inverted): _isInverted(inverted), _debounceval(0), _pressduration(0),_releaseduration(UINT32_MAX),_internalstate(emKeyState::KeyState_Released) {

}
 
Key::emKeyState Key::GetKeyState(){

    return this->_internalstate;
}

Key::keystate Key::UpdateKeyState(bool pysicalstate){
    /* 
    We count how long repeated the key is pressed or released
    We install a software filter for deglitching using a count value as signed one
    - If we press a key we count upwards
    - If we release a key we count downwards
    - If we pass a set threshold we will start counting keypress / keyrelease duration 

    */
    bool has_changed=false; 
    bool logicalvalue;
    if(true == this->_isInverted){
        logicalvalue = !pysicalstate; //Invert signal level
    } else {
        logicalvalue = pysicalstate; 
    }
    if(true == logicalvalue){
        if( (this->_debounceval < INT32_MAX) && (this->_debounceval < this->_debouncelimithigh ) ){
            this->_debounceval++;
            this->_pressduration=0;
        } else {
            /*"HIGH" detected */
            this->_releaseduration =0;
            if(this->_pressduration<UINT32_MAX){
                this->_pressduration++;
            }
        }
    } else {
        if( (this->_debounceval > INT32_MIN) && (this->_debounceval > this->_debouncelimitlow ) ){
            this->_debounceval--;
            this->_releaseduration=0;
        } else {
            /* LOW" detected */
            this->_pressduration=0;
            if(this->_releaseduration<UINT32_MAX){
                this->_releaseduration++;
            }
        }
    }
    

    switch(this->_internalstate){
        case emKeyState::KeyState_Released:{
            /* We can enter long released or pressed from here on*/
            if(this->_releaseduration> 100 ){ /* More than 100 ticks here 100 * 100ms -> 10s */
                this->_internalstate = emKeyState::KeyState_LongReleased;
            } else {
                if((this->_releaseduration<=0) && (this->_pressduration>0)){
                    /* We are no longer in release state .... */
                       this->_internalstate = emKeyState::KeyState_Pressed;
                    /* We need to emitt a press here */ 
                    has_changed=true;
                } else {
                    this->_internalstate = emKeyState::KeyState_Released;
                }
            }
        } break;

        case emKeyState::KeyState_LongReleased:{
            if(this->_releaseduration> 100 ){ /* More than 100 ticks here 100 * 100ms -> 10s */
                this->_internalstate = emKeyState::KeyState_LongReleased;
            } else {
                if((this->_releaseduration<=0) && (this->_pressduration>0)){
                    /* We are no longer in release state .... */
                       this->_internalstate = emKeyState::KeyState_Pressed;
                    /* We need to emitt a press here */ 
                    has_changed=true;
                } else {
                    this->_internalstate = emKeyState::KeyState_Released;
                }
            }
        } break;


        case emKeyState::KeyState_Pressed:{
            if(this->_pressduration> 100 ){ /* More than 100 ticks here 100 * 100ms -> 10s */
                this->_internalstate = emKeyState::KeyState_LongPressed;
                /* We need to emitt a longpress here */  
            } else {
                if((this->_pressduration<=0) && (this->_releaseduration>0)){
                    /* We are no longer in pressed state .... */
                       this->_internalstate = emKeyState::KeyState_Released;
                    /* We need to emitt a release here */
                    has_changed=true;                       
                } else {
                    this->_internalstate = emKeyState::KeyState_Pressed;
                }
            }
        } break;

        case emKeyState::KeyState_LongPressed:{
            if(this->_pressduration> 100 ){ /* More than 100 ticks here 100 * 100ms -> 10s */
                this->_internalstate = emKeyState::KeyState_LongPressed;
            } else {
                if((this->_pressduration<=0) && (this->_releaseduration>0)){
                    /* We are no longer in pressed state .... */
                       this->_internalstate = emKeyState::KeyState_Released;
                    /* We need to emitt a relase here */
                    has_changed=true;                       
                } else {
                    this->_internalstate = emKeyState::KeyState_Pressed;
                }
            }
        } break;
    }

    keystate state;
    state.state = this->_internalstate;
    state.has_changed = has_changed;
    return state;

}
