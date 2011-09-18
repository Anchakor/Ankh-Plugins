/*
  LV2 Ankh Plugins
  Copyright 2006-2011 Jiří Procházka <ojirio@gmail.com>,
                      David Robillard <d@drobilla.net>
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "LV2Plugin.hpp"
#include "distortion.cpp"


#define ANKHPLUGIN ANKHDistortion
#define ANKHPLUG_URI "http://mud.cz/lv2/plugins/ankh-distortion"
#define NPARAMETERS 4
#define NINSOUTS 4


class ANKHPLUGIN : public LV2::Plugin<ANKHPLUGIN> {
private:
    float hpfOldInL, hpfOldOutL, hpfOldInR, hpfOldOutR;
    Ankh::Distortion *distl, *distr;

protected:

public:
    ANKHPLUGIN(double sample_rate, const char*, const LV2::Feature* const*) : LV2::Plugin<ANKHPLUGIN>(NPARAMETERS+NINSOUTS) {
        hpfOldInL = 0.0; hpfOldOutL = 0.0;
        distl = new Ankh::Distortion(sample_rate, &hpfOldInL, &hpfOldOutL);
        hpfOldInR = 0.0; hpfOldOutR = 0.0;
        distr = new Ankh::Distortion(sample_rate, &hpfOldInR, &hpfOldOutR);
    }

    ~ANKHPLUGIN() {
        delete distl; delete distr;
    }
    
    void run(uint32_t sample_count) {
        for(uint32_t sample=0; sample < sample_count; sample++){
            distl->process(p<float>(0)+sample, 
                    p<float>(2)+sample, 
                    p<float>(4), 
                    p<float>(5), 
                    p<float>(6), 
                    p<float>(7));
            distr->process(p<float>(1)+sample, 
                    p<float>(3)+sample, 
                    p<float>(4), 
                    p<float>(5), 
                    p<float>(6), 
                    p<float>(7));
        }
    }
};

static unsigned _ = ANKHPLUGIN::register_class(ANKHPLUG_URI);

