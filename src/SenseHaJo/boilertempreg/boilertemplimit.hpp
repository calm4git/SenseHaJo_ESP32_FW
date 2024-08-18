#include "Arduino.h"

struct boilertemplimits {
    private:
        float _min;
        float _max;

    public: 
        enum emResult {okay, overtemp, undertemp};
        boilertemplimits(float min, float max){
            if(min>max){
                _min=max;
                _max=min;
            } 

            if(min==max){
                _max=max;
                _min=min;
            } 
        }

        void setLimitMax(float max){
            if(max>_min){
                _max=max;
            } else {
                _min=max;
            }
        }

        void setLimitMin(float min){
            if(min>_max){
                _max=min;
            } else {
                _min=min;
            }
        }

        emResult checklimit(float value){
            if( value > _max){
                return overtemp;
            }

            if(value < _min){
                return undertemp;
            }

            return okay;
        }
        

};