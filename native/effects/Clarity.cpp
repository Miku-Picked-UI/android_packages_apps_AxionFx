/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Clarity.h"

#include <algorithm>
#include <cmath>

namespace axionfx {

void Clarity::configure(float sampleRate) {
    mSampleRate = sampleRate;
    for (int c = 0; c < 2; ++c) {
        mFast[c].configure(sampleRate, 0.5f, 25.0f);
        mSlow[c].configure(sampleRate, 20.0f, 250.0f);
    }
    updateFilters();
}

void Clarity::updateFilters() {
    const float nyq = mSampleRate * 0.45f;
    const float a = mAmount;
    for (int c = 0; c < 2; ++c) {
        mShelf[c].configure(BiquadType::HIGH_SHELF, mSampleRate, std::min(7000.0f, nyq), 9.0f * a, 0.6f);
        mPresence[c].configure(BiquadType::PEAKING, mSampleRate, std::min(3000.0f, nyq), 2.5f * a, 0.8f);
        mHighBand[c].configure(BiquadType::HIGHPASS, mSampleRate, std::min(5000.0f, nyq), 0.0f, 0.707f);
        mHarmonicHp[c].configure(BiquadType::HIGHPASS, mSampleRate, std::min(5000.0f, nyq), 0.0f, 0.707f);
        mHarmonicSmooth[c].configure(BiquadType::LOWPASS, mSampleRate, std::min(16000.0f, nyq), 0.0f, 0.707f);
        mAir[c].configure(BiquadType::HIGH_SHELF, mSampleRate, std::min(12000.0f, nyq), 4.0f * a, 0.7f);
        mTransientBand[c].configure(BiquadType::HIGHPASS, mSampleRate, std::min(2000.0f, nyq), 0.0f, 0.707f);
    }
}

void Clarity::setMode(int mode) {
    mMode = std::clamp(mode, static_cast<int>(NATURAL), static_cast<int>(XHIFI));
    reset();
}

void Clarity::setAmount(float percent) {
    mAmount = std::clamp(percent / 100.0f, 0.0f, 1.0f);
    updateFilters();
}

void Clarity::process(float* buffer, int frames) {
    if (!mEnabled || frames <= 0 || mAmount <= 0.0f) return;

    for (int i = 0; i < frames; ++i) {
        for (int c = 0; c < 2; ++c) {
            float x = buffer[i * 2 + c];
            float y = x;
            switch (mMode) {
                case NATURAL:
                    y = mPresence[c].process(mShelf[c].process(x));
                    break;
                case OZONE: {
                    // Even harmonics: full-wave rectify the high band, then high-pass it
                    // again to drop the DC and low products, and smooth the very top.
                    float h = mHighBand[c].process(x);
                    float harm = mHarmonicSmooth[c].process(mHarmonicHp[c].process(std::fabs(h)));
                    y = mAir[c].process(x) + harm * 0.8f * mAmount;
                    break;
                }
                case XHIFI: {
                    // Push the part of the upper band that rises faster than its average
                    // (attacks), which sharpens detail without a constant treble boost.
                    float band = mTransientBand[c].process(x);
                    float level = std::fabs(band);
                    float fast = mFast[c].process(level);
                    float slow = mSlow[c].process(level);
                    float transient = slow > 1e-6f ? std::max(0.0f, fast / slow - 1.0f) : 0.0f;
                    float gain = std::min(transient, 2.0f) * 1.5f * mAmount;
                    y = x + band * gain;
                    break;
                }
            }
            buffer[i * 2 + c] = y;
        }
    }
}

void Clarity::reset() {
    for (int c = 0; c < 2; ++c) {
        mShelf[c].reset();
        mPresence[c].reset();
        mHighBand[c].reset();
        mHarmonicHp[c].reset();
        mHarmonicSmooth[c].reset();
        mAir[c].reset();
        mTransientBand[c].reset();
        mFast[c].reset();
        mSlow[c].reset();
    }
}

}  // namespace axionfx
