/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "../dsp/Biquad.h"

namespace axionfx {

/**
 * Speaker optimization: corrects for a small built-in phone speaker. Original
 * implementation (the idea follows ViPER4Android's speaker optimization switch, no code
 * from it is used).
 *
 *  - high pass below what the speaker can play: that energy only causes distortion and
 *    eats headroom, so removing it lets the rest play louder and cleaner
 *  - low-mid body around the lowest frequencies the speaker does reproduce
 *  - a dip at the typical small-speaker resonance / harshness around 3 kHz
 *  - a little air on top
 *
 * It only processes while the output is the phone speaker (setSpeakerActive).
 */
class SpeakerOptimizer {
public:
    void configure(float sampleRate);
    void process(float* buffer, int frames);
    void setEnabled(bool enabled) { mEnabled = enabled; }
    bool isEnabled() const { return mEnabled; }
    void setSpeakerActive(bool active) { mSpeakerActive = active; }
    void setAmount(float percent);
    void reset();

private:
    void updateFilters();

    bool mEnabled = false;
    bool mSpeakerActive = true;
    float mSampleRate = 48000.0f;
    float mAmount = 0.6f;  // 0..1

    Biquad mHighPass1[2];
    Biquad mHighPass2[2];
    Biquad mBody[2];
    Biquad mHarshDip[2];
    Biquad mAir[2];
};

}  // namespace axionfx
