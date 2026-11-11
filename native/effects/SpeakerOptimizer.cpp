/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

#include "SpeakerOptimizer.h"

#include <algorithm>
#include <cstdint>
#include <cstring>

namespace axionfx {

void SpeakerOptimizer::configure(float sampleRate) {
    mViper.SetSamplingRate(static_cast<uint32_t>(sampleRate));
    mViper.SetEnable(true);  // AxionFx decides whether process() runs
    mViper.Reset();
}

void SpeakerOptimizer::setAmount(float percent) {
    mAmount = std::clamp(percent / 100.0f, 0.0f, 1.0f);
}

void SpeakerOptimizer::process(float* buffer, int frames) {
    if (!mEnabled || !mSpeakerActive || frames <= 0 || mAmount <= 0.0f) return;
    float dry[kChunk * 2];
    const float wet = mAmount;
    for (int done = 0; done < frames; done += kChunk) {
        const int n = std::min(kChunk, frames - done);
        float* chunk = buffer + done * 2;
        std::memcpy(dry, chunk, sizeof(float) * n * 2);
        mViper.Process(chunk, static_cast<uint32_t>(n));
        // ViPER's correction halves the level (about -5 dB in the mids); make that up so the
        // speaker gets the correction's shape (no rumble below 80 Hz, fuller 420 Hz, softer
        // top) at the same loudness instead of just sounding quieter.
        for (int i = 0; i < n * 2; ++i) {
            const float corrected = chunk[i] * kMakeupGain;
            chunk[i] = dry[i] + wet * (corrected - dry[i]);
        }
    }
}

void SpeakerOptimizer::reset() {
    mViper.Reset();
}

}  // namespace axionfx
