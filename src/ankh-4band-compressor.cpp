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
#include "filters.h"


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

    filterLPLR lp1, lp2, lp3;
    filterHPLR hp1, hp2, hp3;
    filterAPLR ap11, ap12, ap2, ap3, ap41, ap42;
    float freqOld[3];

protected:

public:
    ANKHPLUGIN(float sample_rate, const char*, const LV2::Feature* const*) : LV2::Plugin<ANKHPLUGIN>(NPARAMETERS+NINSOUTS) {
        samplerate = sample_rate;
        lp1.setSampleRate(sample_rate);
        lp2.setSampleRate(sample_rate);
        lp3.setSampleRate(sample_rate);
        hp1.setSampleRate(sample_rate);
        hp2.setSampleRate(sample_rate);
        hp3.setSampleRate(sample_rate);
        ap11.setSampleRate(sample_rate);
        ap12.setSampleRate(sample_rate);
        ap2.setSampleRate(sample_rate);
        ap3.setSampleRate(sample_rate);
        ap41.setSampleRate(sample_rate);
        ap42.setSampleRate(sample_rate);

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
            lp1.setCoeffs(*(p<float>(4)), 0.7071);
            hp1.setCoeffs(*(p<float>(4)), 0.7071);
            ap3.setCoeffs(*(p<float>(4)), 0.7071);
            ap41.setCoeffs(*(p<float>(4)), 0.7071);
            freqOld[0] = *p<float>(4);
        }
        if(*p<float>(5) != freqOld[1]) {
            lp2.setCoeffs(*(p<float>(5)), 0.7071);
            hp2.setCoeffs(*(p<float>(5)), 0.7071);
            ap11.setCoeffs(*(p<float>(5)), 0.7071);
            ap42.setCoeffs(*(p<float>(5)), 0.7071);
            freqOld[1] = *p<float>(5);
        }
        if(*p<float>(6) != freqOld[2]) {
            lp3.setCoeffs(*(p<float>(6)), 0.7071);
            hp3.setCoeffs(*(p<float>(6)), 0.7071);
            ap2.setCoeffs(*(p<float>(6)), 0.7071);
            ap12.setCoeffs(*(p<float>(6)), 0.7071);
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
                    lp1.process(left, right);
                    ap11.process(left, right);
                    ap12.process(left, right);
                    outl += left;
                    outr += right;
                } else if(1 == i) {
                    comp2->process(left, right);
                    hp1.process(left, right);
                    lp2.process(left, right);
                    ap2.process(left, right);
                    outl += left;
                    outr += right;
                } else if(2 == i) {
                    comp3->process(left, right);
                    hp2.process(left, right);
                    lp3.process(left, right);
                    ap3.process(left, right);
                    outl += left;
                    outr += right;
                } else if(3 == i) {
                    comp4->process(left, right);
                    hp3.process(left, right);
                    ap41.process(left, right);
                    ap42.process(left, right);
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

