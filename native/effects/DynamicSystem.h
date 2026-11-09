/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 *
 * Dynamic System: level-dependent bass enhancement tuned per listening device (an original
 * implementation of the kind of "dynamic system" found in ViPER4Android). Quiet bass is
 * lifted more than loud bass, so the low end fills out without overloading, and a small
 * low-mid cut keeps it tight. The profile picks the bass and low-mid frequencies.
 */
#pragma once

#include "../dsp/Biquad.h"
#include "../dsp/EnvelopeFollower.h"

namespace axionfx {

class DynamicSystem {
public:
    void configure(float sampleRate);
    void process(float* buffer, int frames);
    void setEnabled(bool enabled) { mEnabled = enabled; }
    bool isEnabled() const { return mEnabled; }
    void setProfile(int profile);
    void setStrength(float percent);
    void setCustomBassHz(float hz);
    void setCustomLowMidHz(float hz);
    void reset();

    static constexpr int kProfileCount = 8;  // the last one is Custom
    static constexpr int kCustomProfile = kProfileCount - 1;

private:
    void updateFilters();
    void updateShelves(float gainDb);
    float bassHz() const;

    bool mEnabled = false;
    float mSampleRate = 48000.0f;
    int mProfile = 0;
    float mStrength = 0.5f;  // 0..1
    float mGainDb = 0.0f;    // smoothed bass boost in dB
    float mShelfDb = -1.0f;  // boost the shelves are currently set to
    float mCustomBassHz = 100.0f;
    float mCustomLowMidHz = 300.0f;

    Biquad mLow1[2];   // side chain: measures the bass level
    Biquad mLow2[2];
    Biquad mShelf[2];  // the bass boost itself (minimum phase, so it never cancels)
    Biquad mLowMidCut[2];
    EnvelopeFollower mEnvelope;
    float mGainSmoothing = 0.001f;  // per block of kBlock frames
    int mBlockPos = 0;
    static constexpr int kBlock = 32;
};

}  // namespace axionfx
