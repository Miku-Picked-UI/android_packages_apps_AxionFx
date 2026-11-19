/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <atomic>

#include "../dsp/Biquad.h"

namespace axionfx {

/**
 * The 9-band "Equalizer" app's stage: one octave-wide peaking filter per octave from 63 Hz to
 * 16 kHz, so neighbouring bands blend into a smooth curve.
 *
 * Band gains are set from the parameter thread and only picked up on the audio thread, moving
 * at most MAX_STEP_DB per block, so dragging a band neither clicks nor races the filters.
 */
class SoundEqualizer {
public:
    static constexpr int NUM_BANDS = 9;
    static constexpr float CENTER_FREQS[NUM_BANDS] = {
        63.0f, 125.0f, 250.0f, 500.0f, 1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f
    };
    static constexpr float Q = 1.41f;  // one octave
    static constexpr float MIN_GAIN_DB = -12.0f;
    static constexpr float MAX_GAIN_DB = 12.0f;
    static constexpr float MAX_STEP_DB = 0.5f;

    void configure(float sampleRate);
    void setBandGain(int band, float dB);
    void setEnabled(bool enabled) { mEnabled.store(enabled); }
    bool isEnabled() const { return mEnabled.load(); }
    void process(float* buffer, int frames);

private:
    void updateBand(int band);

    std::atomic<bool> mEnabled{false};
    std::atomic<bool> mReconfigure{false};
    float mSampleRate = 48000.0f;
    std::atomic<float> mTargetDb[NUM_BANDS] = {};
    float mCurrentDb[NUM_BANDS] = {};
    Biquad mFiltersL[NUM_BANDS];
    Biquad mFiltersR[NUM_BANDS];
};

}  // namespace axionfx
