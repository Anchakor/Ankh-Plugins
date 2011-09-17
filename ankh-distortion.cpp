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
#include <stdio.h>
#include <samplerate.h>

#include "LV2Plugin.hpp"

#define RESAMPLES 1024000

#define ANKHPLUGIN ANKHDistortion
#define ANKHPLUG_URI "http://mud.cz/lv2/plugins/ankh-distortion"
#define NPARAMETERS 4
#define NINSOUTS 4

class ANKHPLUGIN : public LV2::Plugin<ANKHPLUGIN> {
private:
    float toutl, toutr;
    float samplerate, oldsoftclip, sclip;
    float hpfOldInL, hpfOldOutL, hpfOldInR, hpfOldOutR;
    float resampledDataL[RESAMPLES];
    float resampledDataOutL[RESAMPLES];
    float resampledDataR[RESAMPLES];
    float resampledDataOutR[RESAMPLES];
    SRC_DATA src;

protected:
    float hardclip(float in) {
        if(in > 1.0) {
            return 1.0;
        } else if(in < -1.0) {
            return -1.0;
        } else
            return in;
    }

    float hpf(float in, float& oldin, float& oldout) {
        // y(n) = x(n) - x(n-1) + R * y(n-1) 
        // (-3dB @ 30Hz): R = 1-(190/samplerate) // R = 1 - (pi*2 * frequency /samplerate) (pi=3.14159265358979)
        
        float R = 1 - (3.0 / samplerate);
        float out = (in + R * oldin) - oldin;   //float out = in - oldin + R * oldout;
        oldin = in + R * oldin;                 //oldin = in; oldout = out;
        return out;
    }

    void ankhprocess(float *inl, float *inr, float *outl, float *outr, 
            float *mix, float *dcoffset, float *softclip, float *hardclipgain) {
        toutl = *inl;
        toutr = *inr;

        // soft clipping
        if(oldsoftclip != *softclip) {
            sclip = pow(*softclip * 30, 2.0);
            oldsoftclip = *softclip;
        }
        if(sclip > 1e-30) {
            toutl = (1.0 / atan(sclip)) * atan(*dcoffset + (toutl * sclip));
            toutr = (1.0 / atan(sclip)) * atan(*dcoffset + (toutr * sclip));
        }

        // 1 pole HPF cutting < 1Hz to fix the dc offset
        toutl = hpf(toutl, hpfOldInL, hpfOldOutL);
        toutr = hpf(toutr, hpfOldInR, hpfOldOutR);

        // gain
        toutl = (1 + *hardclipgain * 30) * toutl;
        toutr = (1 + *hardclipgain * 30) * toutr;

        // hard clipping
        toutl = hardclip(toutl);
        toutr = hardclip(toutr);

        // mixing
        *outl = (*mix * toutl) + ((1 - *mix) * *inl);
        *outr = (*mix * toutr) + ((1 - *mix) * *inr);
    }

public:
    ANKHPLUGIN(double sample_rate, const char*, const LV2::Feature* const*) : LV2::Plugin<ANKHPLUGIN>(NPARAMETERS+NINSOUTS) {
        samplerate = sample_rate;
        hpfOldInL = 0.0; hpfOldOutL = 0.0;
        hpfOldInR = 0.0; hpfOldOutR = 0.0;
    }
    
    void run(uint32_t sample_count) {
        double resampleRatio = 2.0;
        
        uint32_t pos = 0;
        int r;

        while(pos < sample_count) {
            src.src_ratio = 1.0 / resampleRatio;
            src.input_frames = sample_count - pos;
            src.output_frames = RESAMPLES;

            src.data_in = p<float>(0)+pos;
            src.data_out = resampledDataL;
            r = src_simple(&src, SRC_SINC_FASTEST, 1);
            if(r != 0) { fprintf(stderr, "%s\n", src_strerror(r)); return; }

            src.data_in = p<float>(1)+pos;
            src.data_out = resampledDataR;
            r = src_simple(&src, SRC_SINC_FASTEST, 1);
            if(r != 0) { fprintf(stderr, "%s\n", src_strerror(r)); return; }

            uint32_t newpos = pos + src.input_frames_used;
            for(int sample=0; sample < src.output_frames_gen; sample++){
                ankhprocess(resampledDataL+sample, 
                        resampledDataR+sample, 
                        resampledDataOutL+sample, 
                        resampledDataOutR+sample, 
                        p<float>(4), 
                        p<float>(5), 
                        p<float>(6), 
                        p<float>(7));
            }
            src.src_ratio = resampleRatio;
            src.input_frames = src.output_frames_gen;
            src.output_frames = sample_count - pos;

            src.data_in = resampledDataOutL;
            src.data_out = p<float>(3)+pos;
            r = src_simple(&src, SRC_SINC_FASTEST, 1);
            if(r != 0) { fprintf(stderr, "%s\n", src_strerror(r)); return; }

            src.data_in = resampledDataOutR;
            src.data_out = p<float>(4)+pos;
            r = src_simple(&src, SRC_SINC_FASTEST, 1);
            if(r != 0) { fprintf(stderr, "%s\n", src_strerror(r)); return; }

            pos = newpos;
        }
    }
};

static unsigned _ = ANKHPLUGIN::register_class(ANKHPLUG_URI);

