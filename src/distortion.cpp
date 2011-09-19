/*
 * LV2 Ankh Plugins
 * Copyright 2011 Jiří Procházka <ojirio@gmail.com>
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

// maybe get approximations for x > 1 & x < -1
#define F_ATAN(x) (x > 1.0 ? 1.0 : (x < -1.0 ? -1.0 : \
            (.0318159928972*(2*x-1)+.950551425796+3.86835495723/((2*x-1)+8.05475522951+39.4241153441/((2*x-1)-2.08140771798-.277672591210/((2*x-1)-8.27402153865+95.3157060344/((2*x-1)+10.5910515515)))))\
            ))

namespace Ankh {

class Distortion {
private:
    float tout;
    float samplerate, oldsoftclip, sclip;
    float hpfOldIn, hpfOldOut;

public:
    Distortion(double sample_rate) {
        samplerate = sample_rate;
        hpfOldIn = 0.0; hpfOldOut = 0.0;
    }

    float hardclip(float in) {
        if(in > 1.0) {
            return 1.0;
        } else if(in < -1.0) {
            return -1.0;
        } else
            return in;
    }

    float hpf(float in) {
        // y(n) = x(n) - x(n-1) + R * y(n-1) 
        // (-3dB @ 30Hz): R = 1-(190/samplerate) // R = 1 - (pi*2 * frequency /samplerate) (pi=3.14159265358979)
        
        float R = 1 - (3.0 / samplerate);
        float out = (in + R * hpfOldIn) - hpfOldIn;   //float out = in - hpfOldIn + R * hpfOldOut;
        hpfOldIn = in + R * hpfOldIn;                 //hpfOldIn = in; hpfOldOut = out;
        return out;
    }

    void process(float *in, float *out, float *mix, float *dcoffset, float *softclip, float *hardclipgain) {
        tout = *in;

        // soft clipping
        if(oldsoftclip != *softclip) {
            sclip = pow(*softclip * 30, 2.0);
            oldsoftclip = *softclip;
        }
        if(sclip > 1e-30) {
            //tout = (1.0 / F_ATAN(sclip)) * F_ATAN(*dcoffset + (tout * sclip));
            tout = (1.0 / atan(sclip)) * atan(*dcoffset + (tout * sclip));
            //tout = tanh(*dcoffset + (tout * sclip));
        }

        // 1 pole HPF cutting < 1Hz to fix the dc offset
        tout = hpf(tout);

        // gain
        tout = (1 + *hardclipgain * 30) * tout;

        // hard clipping
        tout = hardclip(tout);

        // mixing
        *out = (*mix * tout) + ((1 - *mix) * *in);
    }

};

}
