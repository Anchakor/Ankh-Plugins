/*
  LV2 Amp Example Plugin
  Copyright 2006-2011 Steve Harris <steve@plugin.org.uk>,
                      David Robillard <d@drobilla.net>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "lv2.h"
#include "LV2Plugin.hpp"


#define ANKHPLUGIN ANKHDistortion
#define ANKHPLUG_URI "http://mud.cz/lv2/plugins/ankh-distortion"
#define NPARAMETERS 5
#define NINSOUTS 4


class ANKHPLUGIN : public LV2::Plugin<ANKHPLUGIN> {
private:
    float toutl, toutr;
    float samplerate, *mix, *softclip, oldsoftclip, sclip, *hardclipgain, *hardclipangle, *dcoffset;
    bool hardclipped;
    int hcaseq;

protected:
    float hardclip(float in) {
        if(in > 1.0) {
            if(hardclipped) {
                in = 1.0 - (*hardclipangle * hcaseq) / samplerate;
                hcaseq++;
            } else {
                in = 1.0;
                hcaseq = 1;
            }
            hardclipped = true;
        } else hardclipped = false;
        return in;
    }

    void ankhprocess(float *inl, float *inr, float *outl, float *outr, 
            float *mix, float *softclip, float *hardclipgain, float *hardclipangle, float *dcoffset) {
        toutl = *inl;
        toutr = *inr;

        // soft clipping
        if(oldsoftclip != *softclip) {
            sclip = pow(*softclip, 2.0);
            oldsoftclip = *softclip;
        }
        if(*softclip > 1e-20) {
        toutl = (1.0 / atan(sclip)) * atan(toutl * (sclip));
        toutr = (1.0 / atan(sclip)) * atan(toutr * (sclip));
        }

        // gain
        toutl = *hardclipgain * toutl;
        toutr = *hardclipgain * toutr;

        // dc offset
        toutl = *dcoffset + toutl;
        toutr = *dcoffset + toutr;

        // hard clipping
        toutl = hardclip(toutl);
        toutr = hardclip(toutr);

        // mixing TODO
        *outl = toutl;
        *outr = toutr;
    }

public:
    ANKHPLUGIN(double sample_rate, const char*, const LV2::Feature* const*) : LV2::Plugin<ANKHPLUGIN>(NPARAMETERS+NINSOUTS) {
        samplerate = sample_rate;
        hcaseq = 1;
    }
    
    void run(uint32_t sample_count) {
        for(uint32_t sample=0; sample < sample_count; sample++){
            ankhprocess(p<float>(0)+sample, 
                    p<float>(1)+sample, 
                    p<float>(2)+sample, 
                    p<float>(3)+sample, 
                    p<float>(4), 
                    p<float>(5), 
                    p<float>(6), 
                    p<float>(7), 
                    p<float>(8));
        }
    }
};

static unsigned _ = ANKHPLUGIN::register_class(ANKHPLUG_URI);

