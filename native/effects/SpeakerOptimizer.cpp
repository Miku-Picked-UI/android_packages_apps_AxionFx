/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

#include "SpeakerOptimizer.h"

#include <algorithm>

namespace axionfx {

namespace {
constexpr float kHighPassHz = 110.0f;  // below this a phone speaker produces mostly distortion
constexpr float kBodyHz = 320.0f;
constexpr float kHarshHz = 3200.0f;
constexpr float kAirHz = 9000.0f;
}  // namespace

void SpeakerOptimizer::configure(float sampleRate) {
    mSampleRate = sampleRate;
    updateFilters();
}

void SpeakerOptimizer::setAmount(float percent) {
    mAmount = std::clamp(percent / 100.0f, 0.0f, 1.0f);
    updateFilters();
}

void SpeakerOptimizer::updateFilters() {
    const float nyq = mSampleRate * 0.45f;
    for (int c = 0; c < 2; ++c) {
        // 4th order (two 2nd order Butterworth) high pass, always on while active
        mHighPass1[c].configure(BiquadType::HIGHPASS, mSampleRate, kHighPassHz, 0.0f, 0.541f);
        mHighPass2[c].configure(BiquadType::HIGHPASS, mSampleRate, kHighPassHz, 0.0f, 1.307f);
        mBody[c].configure(BiquadType::PEAKING, mSampleRate, kBodyHz, 3.5f * mAmount, 0.8f);
        mHarshDip[c].configure(BiquadType::PEAKING, mSampleRate, std::min(kHarshHz, nyq),
                               -3.0f * mAmount, 1.4f);
        mAir[c].configure(BiquadType::HIGH_SHELF, mSampleRate, std::min(kAirHz, nyq),
                          2.5f * mAmount, 0.707f);
    }
}

void SpeakerOptimizer::process(float* buffer, int frames) {
    if (!mEnabled || !mSpeakerActive || frames <= 0) return;
    for (int i = 0; i < frames; ++i) {
        for (int c = 0; c < 2; ++c) {
            float x = buffer[i * 2 + c];
            x = mHighPass2[c].process(mHighPass1[c].process(x));
            x = mAir[c].process(mHarshDip[c].process(mBody[c].process(x)));
            buffer[i * 2 + c] = x;
        }
    }
}

void SpeakerOptimizer::reset() {
    for (int c = 0; c < 2; ++c) {
        mHighPass1[c].reset();
        mHighPass2[c].reset();
        mBody[c].reset();
        mHarshDip[c].reset();
        mAir[c].reset();
    }
}

}  // namespace axionfx
