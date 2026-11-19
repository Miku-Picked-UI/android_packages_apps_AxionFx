/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

#include "effects/SoundEqualizer.h"

#include <algorithm>
#include <cmath>

namespace axionfx {

constexpr float SoundEqualizer::CENTER_FREQS[];

void SoundEqualizer::configure(float sampleRate) {
    mSampleRate = sampleRate;
    mReconfigure.store(true);
}

void SoundEqualizer::setBandGain(int band, float dB) {
    if (band < 0 || band >= NUM_BANDS) return;
    mTargetDb[band].store(std::clamp(dB, MIN_GAIN_DB, MAX_GAIN_DB));
}

void SoundEqualizer::updateBand(int band) {
    if (std::fabs(mCurrentDb[band]) < 0.05f) {
        mFiltersL[band].setPassthrough();
        mFiltersR[band].setPassthrough();
        return;
    }
    // Stay clear of Nyquist at low sample rates.
    const float freq = std::min(CENTER_FREQS[band], mSampleRate * 0.45f);
    mFiltersL[band].configure(BiquadType::PEAKING, mSampleRate, freq, mCurrentDb[band], Q);
    mFiltersR[band].configure(BiquadType::PEAKING, mSampleRate, freq, mCurrentDb[band], Q);
}

void SoundEqualizer::process(float* buffer, int frames) {
    if (!mEnabled.load() || frames <= 0) return;

    const bool all = mReconfigure.exchange(false);
    if (all) {
        for (int b = 0; b < NUM_BANDS; b++) {
            mFiltersL[b].reset();
            mFiltersR[b].reset();
        }
    }
    for (int b = 0; b < NUM_BANDS; b++) {
        const float target = mTargetDb[b].load();
        const float delta = target - mCurrentDb[b];
        if (delta != 0.0f) {
            mCurrentDb[b] += std::clamp(delta, -MAX_STEP_DB, MAX_STEP_DB);
            updateBand(b);
        } else if (all) {
            updateBand(b);
        }
    }

    for (int f = 0; f < frames; f++) {
        float left = buffer[f * 2];
        float right = buffer[f * 2 + 1];
        for (int b = 0; b < NUM_BANDS; b++) {
            left = mFiltersL[b].process(left);
            right = mFiltersR[b].process(right);
        }
        buffer[f * 2] = left;
        buffer[f * 2 + 1] = right;
    }
}

}  // namespace axionfx
