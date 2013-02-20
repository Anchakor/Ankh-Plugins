/*
 * LV2 Ankh Plugins
 * Copyright 2006-2011 Jiří Procházka <ojirio@gmail.com>
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

#include "biquad.h"

/// filter base
class filterBase
{
protected:
    double samplerate;

public:
    filterBase() {};
    virtual void setSampleRate(double sample_rate) {
        samplerate = sample_rate;
    }
    virtual void process(float &leftChan, float &rightChan) = 0;
};

/// linkwitz riley filter base
class filterLRbase : public filterBase
{
protected:
    dsp::biquad_d2<float> L0n0, R0n0, L0n1, R0n1;

public:
    filterLRbase() : filterBase() { }
    virtual void setCoeffs(float freq, float Q) = 0;
    virtual void process(float &leftChan, float &rightChan) {
        leftChan = L0n0.process(leftChan);
        leftChan = L0n1.process(leftChan);
        rightChan = R0n0.process(rightChan);
        rightChan = R0n1.process(rightChan);
        L0n0.sanitize();
        L0n1.sanitize();
        R0n0.sanitize();
        R0n1.sanitize();
    }
};

/// low pass linkwitz riley filter
class filterLPLR : public filterLRbase
{
public:
    //filterLPLR(double sample_rate) : filterLRbase(sample_rate) { }
    virtual void setCoeffs(float freq, float Q) {
        L0n0.set_lp_rbj(freq, Q, (float)samplerate);
        R0n0.copy_coeffs(L0n0);
        L0n1.copy_coeffs(L0n0);
        R0n1.copy_coeffs(L0n0);
    }
};

/// high pass linkwitz riley filter
class filterHPLR : public filterLRbase
{
public:
    filterHPLR() : filterLRbase() { }
    virtual void setCoeffs(float freq, float Q) {
        L0n0.set_hp_rbj(freq, Q, (float)samplerate);
        R0n0.copy_coeffs(L0n0);
        L0n1.copy_coeffs(L0n0);
        R0n1.copy_coeffs(L0n0);
    }
};

/// all pass linkwitz riley filter
class filterAPLR : public filterLRbase
{
private:
    filterLPLR lp;
    filterHPLR hp;
public:
    filterAPLR() : filterLRbase() { }
    virtual void setSampleRate(double sample_rate) {
        samplerate = sample_rate;
        lp.setSampleRate(sample_rate);
        hp.setSampleRate(sample_rate);
    }
    virtual void setCoeffs(float freq, float Q) {
        lp.setCoeffs(freq, Q);
        hp.setCoeffs(freq, Q);
    }
    virtual void process(float &leftChan, float &rightChan) {
        float l1 = leftChan;
        float l2 = leftChan;
        float r1 = rightChan;
        float r2 = rightChan;
        lp.process(l1, r1);
        hp.process(l2, r2);
        leftChan = l1+l2;
        rightChan = r1+r2;
    }
};

/// all pass linkwitz riley filter 2 - doesn't work
/*
class filterAPLR2 : public filterLRbase
{
public:
    filterAPLR2() : filterLRbase() { }
    virtual void setCoeffs(float freq, float Q) {
        L0n0.set_allpass(freq, Q, (float)samplerate);
        R0n0.copy_coeffs(L0n0);
        L0n1.copy_coeffs(L0n0);
        R0n1.copy_coeffs(L0n0);
    }
};
*/


