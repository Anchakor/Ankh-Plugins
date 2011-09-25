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

#define CMAX(a,b) (a>b?a:b)

namespace Ankh {

class Compressor {
private:
	float threshold;
	float attack, release, envelope_decay;
	float output;
	float transfer_A, transfer_B;
	float env, gain;

public:
    Compressor() {
        threshold = 1.0;
		attack = release = envelope_decay = 0.0;
		/* float att = 5.0;
        attack = exp(-1.0/att);
		float rel = 50.0;
	   	release = exp(-1.0/rel);
		envelope_decay = exp(-4.0/rel); */
        output = 1.0;
        transfer_A = 0.0;
		//transfer_B = output * pow(threshold,-transfer_A);
        transfer_B = 1.0;
        env = 0.0;
        gain = 1.0;
    }

    void setThreshold(float value) {
        threshold = value;
        transfer_B = output * pow(threshold,-transfer_A);
    }
    
    void setRatio(float value) {
        transfer_A = value-1.0;
        transfer_B = output * pow(threshold,-transfer_A);
    }
    
    void setAttack(float value) {
        attack = exp(-1.0/value);
    }
    
    void setRelease(float value) {
        release = exp(-1.0/value);
        envelope_decay = exp(-4.0/value); /* = exp(-1/(0.25*value)) */
    }
    
    void setOutput(float value) {
        output = value;
        transfer_B = output * pow(threshold,-transfer_A);
    }
    
    void reset() {
        env = 0.0; gain = 1.0;
    }
    
    inline void process(float *input_left, float *input_right, float *output_left, float *output_right) {
        float det, transfer_gain;
		det = CMAX(fabs(*input_left),fabs(*input_right));
		det += 10e-30; /* add tiny DC offset (-600dB) to prevent denormals */

		env = (det >= env) ? det : det+envelope_decay * (env-det);

		transfer_gain = (env > threshold) ? pow(env,transfer_A) * transfer_B : output;

		gain = (transfer_gain < gain) ?
						transfer_gain + attack * (gain - transfer_gain):
						transfer_gain + release * (gain - transfer_gain);

		*output_left = *input_left * gain;
		*output_right = *input_right * gain;
    }

};

}
