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
#include "biquad.h"


#define ANKHPLUGIN ANKH3BandDistortion
#define ANKHPLUG_URI "http://mud.cz/lv2/plugins/ankh-3band-distortion"
#define NPARAMETERS 17
#define NINSOUTS 4


class ANKHPLUGIN : public LV2::Plugin<ANKHPLUGIN> {
private:
    double samplerate;

    float hpfOldInL1, hpfOldOutL1, hpfOldInR1, hpfOldOutR1;
    Ankh::Distortion *distl1, *distr1;
    float hpfOldInL2, hpfOldOutL2, hpfOldInR2, hpfOldOutR2;
    Ankh::Distortion *distl2, *distr2;
    float hpfOldInL3, hpfOldOutL3, hpfOldInR3, hpfOldOutR3;
    Ankh::Distortion *distl3, *distr3;

    dsp::biquad_d2<float> lpL0, lpR0, lpL1, lpR1, hpL0, hpR0, hpL1, hpR1;
    float freqOld[2];

protected:

public:
    ANKHPLUGIN(double sample_rate, const char*, const LV2::Feature* const*) : LV2::Plugin<ANKHPLUGIN>(NPARAMETERS+NINSOUTS) {
        samplerate = sample_rate;

        hpfOldInL1 = 0.0; hpfOldOutL1 = 0.0;
        distl1 = new Ankh::Distortion(sample_rate, &hpfOldInL1, &hpfOldOutL1);
        hpfOldInR1 = 0.0; hpfOldOutR1 = 0.0;
        distr1 = new Ankh::Distortion(sample_rate, &hpfOldInR1, &hpfOldOutR1);
        hpfOldInL2 = 0.0; hpfOldOutL2 = 0.0;
        distl2 = new Ankh::Distortion(sample_rate, &hpfOldInL2, &hpfOldOutL2);
        hpfOldInR2 = 0.0; hpfOldOutR2 = 0.0;
        distr2 = new Ankh::Distortion(sample_rate, &hpfOldInR2, &hpfOldOutR2);
        hpfOldInL3 = 0.0; hpfOldOutL3 = 0.0;
        distl3 = new Ankh::Distortion(sample_rate, &hpfOldInL3, &hpfOldOutL3);
        hpfOldInR3 = 0.0; hpfOldOutR3 = 0.0;
        distr3 = new Ankh::Distortion(sample_rate, &hpfOldInR3, &hpfOldOutR3);
    }

    ~ANKHPLUGIN() {
        delete distl1; delete distr1;
        delete distl2; delete distr2;
        delete distl3; delete distr3;
    }
    
    void run(uint32_t sample_count) {
        if(*p<float>(4) != freqOld[0]) {
            lpL0.set_lp_rbj(*(p<float>(4)) * (1.0 + 0.17), 1.0, (float)samplerate);
            lpR0.copy_coeffs(lpL0);
            hpL0.set_hp_rbj(*(p<float>(4)) * (1.0 - 0.17), 1.0, (float)samplerate);
            hpR0.copy_coeffs(hpL0);
            freqOld[0] = *p<float>(4);
        }
        if(*p<float>(5) != freqOld[1]) {
            lpL1.set_lp_rbj(*(p<float>(5)) * (1.0 + 0.17), 1.0, (float)samplerate);
            lpR1.copy_coeffs(lpL1);
            hpL1.set_hp_rbj(*(p<float>(5)) * (1.0 - 0.17), 1.0, (float)samplerate);
            hpR1.copy_coeffs(hpL1);
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
                    toutl = lpL0.process(toutl);
                    toutr = lpR0.process(toutr);
                    lpL0.sanitize();
                    lpR0.sanitize();
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
                    toutl = lpL1.process(toutl);
                    toutr = lpR1.process(toutr);
                    toutl = hpL0.process(toutl);
                    toutr = hpR0.process(toutr);
                    lpL1.sanitize();
                    lpR1.sanitize();
                    hpL0.sanitize();
                    hpR0.sanitize();
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
                    toutl = hpL1.process(toutl);
                    toutr = hpR1.process(toutr);
                    hpL1.sanitize();
                    hpR1.sanitize();
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

