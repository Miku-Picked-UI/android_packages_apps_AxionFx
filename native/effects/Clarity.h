/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 *
 * Clarity: brings back treble detail, in three styles (an original implementation; the
 * modes are named after the kinds of clarity processing found in ViPER4Android).
 *  - NATURAL: smooth high shelf with a gentle presence lift.
 *  - OZONE:   high band harmonic enhancement (even harmonics) plus an "air" shelf.
 *  - XHIFI:   transient emphasis of the upper mids and highs.
 */
#pragma once

#include "../dsp/Biquad.h"
#include "../dsp/EnvelopeFollower.h"

namespace axionfx {

class Clarity {
public:
    enum Mode { NATURAL = 0, OZONE = 1, XHIFI = 2 };

    void configure(float sampleRate);
    void process(float* buffer, int frames);
    void setEnabled(bool enabled) { mEnabled = enabled; }
    bool isEnabled() const { return mEnabled; }
    void setMode(int mode);
    void setAmount(float percent);
    void reset();

private:
    void updateFilters();

    bool mEnabled = false;
    float mSampleRate = 48000.0f;
    int mMode = NATURAL;
    float mAmount = 0.5f;  // 0..1

    // NATURAL
    Biquad mShelf[2];
    Biquad mPresence[2];
    // OZONE
    Biquad mHighBand[2];
    Biquad mHarmonicHp[2];
    Biquad mHarmonicSmooth[2];
    Biquad mAir[2];
    // XHIFI
    Biquad mTransientBand[2];
    EnvelopeFollower mFast[2];
    EnvelopeFollower mSlow[2];
};

}  // namespace axionfx
