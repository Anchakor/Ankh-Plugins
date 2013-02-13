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
#include "SimpleComp.h"
#include "biquad.h"


#define ANKHPLUGIN ANKH4BandCompressor
#define ANKHPLUG_URI "http://mud.cz/lv2/plugins/ankh-4band-compressor"
#define NPARAMETERS 12
#define NINSOUTS 4


class ANKHPLUGIN : public LV2::Plugin<ANKHPLUGIN> {
private:
    float samplerate;

    chunkware_simple::SimpleComp *comp1;
    chunkware_simple::SimpleComp *comp2;
    chunkware_simple::SimpleComp *comp3;
    chunkware_simple::SimpleComp *comp4;

    dsp::biquad_d2<float> lpL0n0, lpR0n0, lpL0n1, lpR0n1,
        hpL0n0, hpR0n0, hpL0n1, hpR0n1, lpL1n0, lpR1n0, lpL1n1, lpR1n1, 
        hpL1n0, hpR1n0, hpL1n1, hpR1n1, lpL2n0, lpR2n0, lpL2n1, lpR2n1, 
        hpL2n0, hpR2n0, hpL2n1, hpR2n1;
    float freqOld[3];

protected:

public:
    ANKHPLUGIN(float sample_rate, const char*, const LV2::Feature* const*) : LV2::Plugin<ANKHPLUGIN>(NPARAMETERS+NINSOUTS) {
        samplerate = sample_rate;

        comp1 = new chunkware_simple::SimpleComp();
        comp2 = new chunkware_simple::SimpleComp();
        comp3 = new chunkware_simple::SimpleComp();
        comp4 = new chunkware_simple::SimpleComp();
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
            lpL0n0.set_lp_rbj(*(p<float>(4)) * (1.0 + 0.29), 0.7, (float)samplerate);
            lpR0n0.copy_coeffs(lpL0n0);
            lpL0n1.copy_coeffs(lpL0n0);
            lpR0n1.copy_coeffs(lpL0n0);
            hpL0n0.set_hp_rbj(*(p<float>(4)) * (1.0 - 0.29), 0.7, (float)samplerate);
            hpR0n0.copy_coeffs(hpL0n0);
            hpL0n1.copy_coeffs(hpL0n0);
            hpR0n1.copy_coeffs(hpL0n0);
            freqOld[0] = *p<float>(4);
        }
        if(*p<float>(5) != freqOld[1]) {
            lpL1n0.set_lp_rbj(*(p<float>(5)) * (1.0 + 0.29), 0.7, (float)samplerate);
            lpR1n0.copy_coeffs(lpL1n0);
            lpL1n1.copy_coeffs(lpL1n0);
            lpR1n1.copy_coeffs(lpL1n0);
            hpL1n0.set_hp_rbj(*(p<float>(5)) * (1.0 - 0.29), 0.7, (float)samplerate);
            hpR1n0.copy_coeffs(hpL1n0);
            hpL1n1.copy_coeffs(hpL1n0);
            hpR1n1.copy_coeffs(hpL1n0);
            freqOld[1] = *p<float>(5);
        }
        if(*p<float>(6) != freqOld[2]) {
            lpL2n0.set_lp_rbj(*(p<float>(6)) * (1.0 + 0.29), 0.7, (float)samplerate);
            lpR2n0.copy_coeffs(lpL2n0);
            lpL2n1.copy_coeffs(lpL2n0);
            lpR2n1.copy_coeffs(lpL2n0);
            hpL2n0.set_hp_rbj(*(p<float>(6)) * (1.0 - 0.29), 0.7, (float)samplerate);
            hpR2n0.copy_coeffs(hpL2n0);
            hpL2n1.copy_coeffs(hpL2n0);
            hpR2n1.copy_coeffs(hpL2n0);
            freqOld[2] = *p<float>(6);
        }

        for(int i=0; i < 4; i++) {
            if(0 == i) {
                comp1->setRatio(*(p<float>(7)));
                comp1->setThresh(*(p<float>(11)));
            } else if(1 == i) {
                comp2->setRatio(*(p<float>(8)));
                comp2->setThresh(*(p<float>(12)));
            } else if(2 == i) {
                comp3->setRatio(*(p<float>(9)));
                comp3->setThresh(*(p<float>(13)));
            } else if(3 == i) {
                comp4->setRatio(*(p<float>(10)));
                comp4->setThresh(*(p<float>(14)));
            }
        }
        for(uint32_t sample=0; sample < sample_count; sample++){
            float outl = 0.0;
            float outr = 0.0;
            float left;
            float right;
            for(int i=0; i < 5; i++) {
                left  = *(p<float>(0)+sample);
                right = *(p<float>(1)+sample);
                if(4 == i) { goto outputit;
                } else if(0 == i) {
                    comp1->process(left, right);
                    left = lpL0n0.process(left);
                    left = lpL0n1.process(left);
                    right = lpR0n0.process(right);
                    right = lpR0n1.process(right);
                    lpL0n0.sanitize();
                    lpL0n1.sanitize();
                    lpR0n0.sanitize();
                    lpR0n1.sanitize();
                    outl += left;
                    outr += right;
                } else if(1 == i) {
                    comp2->process(left, right);
                    left = lpL1n0.process(left);
                    left = lpL1n1.process(left);
                    right = lpR1n0.process(right);
                    right = lpR1n1.process(right);
                    left = hpL0n0.process(left);
                    left = hpL0n1.process(left);
                    right = hpR0n0.process(right);
                    right = hpR0n1.process(right);
                    lpL1n0.sanitize();
                    lpL1n1.sanitize();
                    lpR1n0.sanitize();
                    lpR1n1.sanitize();
                    hpL0n0.sanitize();
                    hpL0n1.sanitize();
                    hpR0n0.sanitize();
                    hpR0n1.sanitize();
                    outl += left;
                    outr += right;
                } else if(2 == i) {
                    comp3->process(left, right);
                    left = lpL2n0.process(left);
                    left = lpL2n1.process(left);
                    right = lpR2n0.process(right);
                    right = lpR2n1.process(right);
                    left = hpL1n0.process(left);
                    left = hpL1n1.process(left);
                    right = hpR1n0.process(right);
                    right = hpR1n1.process(right);
                    lpL2n0.sanitize();
                    lpL2n1.sanitize();
                    lpR2n0.sanitize();
                    lpR2n1.sanitize();
                    hpL1n0.sanitize();
                    hpL1n1.sanitize();
                    hpR1n0.sanitize();
                    hpR1n1.sanitize();
                    outl += left;
                    outr += right;
                } else if(3 == i) {
                    comp4->process(left, right);
                    left = hpL2n0.process(left);
                    left = hpL2n1.process(left);
                    right = hpR2n0.process(right);
                    right = hpR2n1.process(right);
                    hpL2n0.sanitize();
                    hpL2n1.sanitize();
                    hpR2n0.sanitize();
                    hpR2n1.sanitize();
                    outl += left;
                    outr += right;
                }
            }
outputit:
            outl = (*(p<float>(15)) * outl) + ((1 - *(p<float>(15))) * left);
            outr = (*(p<float>(15)) * outr) + ((1 - *(p<float>(15))) * right);
            *(p<float>(2)+sample) = hardclip(outl);
            *(p<float>(3)+sample) = hardclip(outr);
        }
    }
};

static unsigned _ = ANKHPLUGIN::register_class(ANKHPLUG_URI);

