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
#include "compressor.cpp"
#include "biquad.h"


#define ANKHPLUGIN ANKH4BandCompressor
#define ANKHPLUG_URI "http://mud.cz/lv2/plugins/ankh-4band-compressor"
#define NPARAMETERS 12
#define NINSOUTS 4


class ANKHPLUGIN : public LV2::Plugin<ANKHPLUGIN> {
private:
    double samplerate;

    Ankh::Compressor *comp1;
    Ankh::Compressor *comp2;
    Ankh::Compressor *comp3;
    Ankh::Compressor *comp4;

    dsp::biquad_d2<float> lpL0, lpR0, lpL1, lpR1, hpL0, hpR0, lpL2, lpR2, hpL1, hpR1, hpL2, hpR2;
    float freqOld[3];

protected:

public:
    ANKHPLUGIN(double sample_rate, const char*, const LV2::Feature* const*) : LV2::Plugin<ANKHPLUGIN>(NPARAMETERS+NINSOUTS) {
        samplerate = sample_rate;

        comp1 = new Ankh::Compressor();
        comp2 = new Ankh::Compressor();
        comp3 = new Ankh::Compressor();
        comp4 = new Ankh::Compressor();
    }

    ~ANKHPLUGIN() {
        delete comp1;
        delete comp2;
        delete comp3;
        delete comp4;
    }

    float hardclip(float in) {
        if(in > 1.0) {
            return 1.0;
        } else if(in < -1.0) {
            return -1.0;
        } else
            return in;
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
        if(*p<float>(6) != freqOld[2]) {
            lpL2.set_lp_rbj(*(p<float>(6)) * (1.0 + 0.17), 1.0, (float)samplerate);
            lpR2.copy_coeffs(lpL2);
            hpL2.set_hp_rbj(*(p<float>(6)) * (1.0 - 0.17), 1.0, (float)samplerate);
            hpR2.copy_coeffs(hpL2);
            freqOld[2] = *p<float>(6);
        }

		for(int i=0; i < 4; i++) {
			if(0 == i) {
				comp1->setRatio(*(p<float>(7)));
				comp1->setThreshold(*(p<float>(11)));
			} else if(1 == i) {
				comp2->setRatio(*(p<float>(8)));
				comp2->setThreshold(*(p<float>(12)));
			} else if(2 == i) {
				comp3->setRatio(*(p<float>(9)));
				comp3->setThreshold(*(p<float>(13)));
			} else if(3 == i) {
				comp4->setRatio(*(p<float>(10)));
				comp4->setThreshold(*(p<float>(14)));
			}
		}
        for(uint32_t sample=0; sample < sample_count; sample++){
            float toutl = 0.0;
            float toutr = 0.0;
            float outl = 0.0;
            float outr = 0.0;
			float left;
			float right;
            for(int i=0; i < 4; i++) {
                left  = *(p<float>(0)+sample);
                right = *(p<float>(1)+sample);
                if(0 == i) {
					comp1->process(&left, &right, &toutl, &toutr);
                    toutl = lpL0.process(toutl);
                    toutr = lpR0.process(toutr);
                    lpL0.sanitize();
                    lpR0.sanitize();
                    outl += toutl;
                    outr += toutr;
                } else if(1 == i) {
					comp2->process(&left, &right, &toutl, &toutr);
                    toutl = lpL1.process(toutl);
                    toutr = lpR1.process(toutr);
                    toutl = hpL0.process(toutl);
                    toutr = hpR0.process(toutr);
                    lpL1.sanitize();
                    lpR1.sanitize();
                    hpL0.sanitize();
                    hpR0.sanitize();
                    outl += toutl;
                    outr += toutr;
                } else if(2 == i) {
					comp3->process(&left, &right, &toutl, &toutr);
                    toutl = lpL2.process(toutl);
                    toutr = lpR2.process(toutr);
                    toutl = hpL1.process(toutl);
                    toutr = hpR1.process(toutr);
                    lpL2.sanitize();
                    lpR2.sanitize();
                    hpL1.sanitize();
                    hpR1.sanitize();
                    outl += toutl;
                    outr += toutr;
                } else if(3 == i) {
					comp4->process(&left, &right, &toutl, &toutr);
                    toutl = hpL2.process(toutl);
                    toutr = hpR2.process(toutr);
                    hpL2.sanitize();
                    hpR2.sanitize();
                    outl += toutl;
                    outr += toutr;
                }
            }
			outl = (*(p<float>(15)) * outl) + ((1 - *(p<float>(15))) * left);
			outr = (*(p<float>(15)) * outr) + ((1 - *(p<float>(15))) * right);
            *(p<float>(2)+sample) = hardclip(outl);
            *(p<float>(3)+sample) = hardclip(outr);
        }
    }
};

static unsigned _ = ANKHPLUGIN::register_class(ANKHPLUG_URI);

