/*
 * LV2 Ankh Plugins
 * Copyright 2006-2011 Jiří Procházka <ojirio@gmail.com>,
 *                     David Robillard <d@drobilla.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "LV2Plugin.hpp"
#include "distortion.cpp"
#include "filters.h"


#define ANKHPLUGIN ANKH3BandDistortion
#define ANKHPLUG_URI "http://mud.cz/lv2/plugins/ankh-3band-distortion"
#define NPARAMETERS 17
#define NINSOUTS 4


class ANKHPLUGIN : public LV2::Plugin<ANKHPLUGIN> {
private:
    double samplerate;

    Ankh::Distortion *distl1, *distr1;
    Ankh::Distortion *distl2, *distr2;
    Ankh::Distortion *distl3, *distr3;

    filterLPLR lp1, lp2;
    filterHPLR hp1, hp2;
    filterAPLR ap1, ap2;
    float freqOld[2];

protected:

public:
    ANKHPLUGIN(double sample_rate, const char*, const LV2::Feature* const*) : LV2::Plugin<ANKHPLUGIN>(NPARAMETERS+NINSOUTS) {
        samplerate = sample_rate;
        lp1.setSampleRate(sample_rate);
        lp2.setSampleRate(sample_rate);
        hp1.setSampleRate(sample_rate);
        hp2.setSampleRate(sample_rate);
        ap1.setSampleRate(sample_rate);
        ap2.setSampleRate(sample_rate);

        distl1 = new Ankh::Distortion(sample_rate);
        distr1 = new Ankh::Distortion(sample_rate);
        distl2 = new Ankh::Distortion(sample_rate);
        distr2 = new Ankh::Distortion(sample_rate);
        distl3 = new Ankh::Distortion(sample_rate);
        distr3 = new Ankh::Distortion(sample_rate);
    }

    ~ANKHPLUGIN() {
        delete distl1; delete distr1;
        delete distl2; delete distr2;
        delete distl3; delete distr3;
    }
    
    void run(uint32_t sample_count) {
        if(*p<float>(4) != freqOld[0]) {
            lp1.setCoeffs(*(p<float>(4)), 0.7071);
            hp1.setCoeffs(*(p<float>(4)), 0.7071);
            ap2.setCoeffs(*(p<float>(4)), 0.7071);
            freqOld[0] = *p<float>(4);
        }
        if(*p<float>(5) != freqOld[1]) {
            lp2.setCoeffs(*(p<float>(5)), 0.7071);
            hp2.setCoeffs(*(p<float>(5)), 0.7071);
            ap1.setCoeffs(*(p<float>(5)), 0.7071);
            freqOld[1] = *p<float>(5);
        }

        for(uint32_t sample=0; sample < sample_count; sample++){
            float toutl = 0.0;
            float toutr = 0.0;
            float outl = 0.0;
            float outr = 0.0;
            for(int i=0; i < 3; i++) {
                float left  = *(p<float>(0)+sample);
                float right = *(p<float>(1)+sample);
                if(0 == i) {
                    distl1->process(&left, 
                            &toutl, 
                            p<float>(9), 
                            p<float>(6), 
                            p<float>(7), 
                            p<float>(8));
                    distr1->process(&right, 
                            &toutr,
                            p<float>(9), 
                            p<float>(6), 
                            p<float>(7), 
                            p<float>(8));
                    lp1.process(toutl, toutr);
                    ap1.process(toutl, toutr);
                    outl += toutl * *p<float>(18);
                    outr += toutr * *p<float>(18);
                } else if(1 == i) {
                    distl2->process(&left, 
                            &toutl, 
                            p<float>(13), 
                            p<float>(10), 
                            p<float>(11), 
                            p<float>(12));
                    distr2->process(&right, 
                            &toutr,
                            p<float>(13), 
                            p<float>(10), 
                            p<float>(11), 
                            p<float>(12));
                    hp1.process(toutl, toutr);
                    lp2.process(toutl, toutr);
                    outl += toutl * *p<float>(19);
                    outr += toutr * *p<float>(19);
                } else if(2 == i) {
                    distl3->process(&left, 
                            &toutl, 
                            p<float>(17), 
                            p<float>(14), 
                            p<float>(15), 
                            p<float>(16));
                    distr3->process(&right, 
                            &toutr,
                            p<float>(17), 
                            p<float>(14), 
                            p<float>(15), 
                            p<float>(16));
                    hp2.process(toutl, toutr);
                    ap2.process(toutl, toutr);
                    outl += toutl * *p<float>(20);
                    outr += toutr * *p<float>(20);
                }
            }
            *(p<float>(2)+sample) = outl;
            *(p<float>(3)+sample) = outr;
        }
    }
};

static unsigned _ = ANKHPLUGIN::register_class(ANKHPLUG_URI);

