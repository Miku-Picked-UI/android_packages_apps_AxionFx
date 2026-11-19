/*
 * Copyright 2025-2026 AxionOS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Limiter.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace axionfx {

void Limiter::configure(float sampleRate) {
    mSampleRate = sampleRate;
    mLookaheadSamples = std::min(static_cast<int>(sampleRate * LOOKAHEAD_MS / 1000.0f),
                                  MAX_LOOKAHEAD_SAMPLES);
    if (mLookaheadSamples < 1) mLookaheadSamples = 1;
    // Miku-Picked UI: reach a new peak's gain smoothly within the lookahead (about 98 % after
    // four time constants) instead of jumping to it in one sample.
    mAttackCoeff = std::exp(-4.0f / static_cast<float>(mLookaheadSamples));

    if (mReleaseMs > 0.0f) {
        mReleaseCoeff = std::exp(-1.0f / (sampleRate * mReleaseMs / 1000.0f));
    } else {
        mReleaseCoeff = 0.0f;
    }

    reset();
}

void Limiter::setThreshold(float dB) {
    dB = std::clamp(dB, -60.0f, 0.0f);
    mThreshold = std::pow(10.0f, dB / 20.0f);
}

void Limiter::setRelease(float ms) {
    mReleaseMs = std::clamp(ms, 1.0f, 500.0f);
    if (mSampleRate > 0.0f) {
        mReleaseCoeff = std::exp(-1.0f / (mSampleRate * mReleaseMs / 1000.0f));
    }
}

void Limiter::process(float* buffer, int frames) {
    if (!mEnabled) return;

    for (int f = 0; f < frames; f++) {
        float inL = buffer[f * 2];
        float inR = buffer[f * 2 + 1];

        float peak = std::max(std::fabs(inL), std::fabs(inR));

        float targetGain = 1.0f;
        if (peak > mThreshold) {
            targetGain = mThreshold / peak;
        }

        // Miku-Picked UI: hold a peak's gain until that peak has left the lookahead delay, ramp
        // down to it smoothly (attack) and recover slowly (release). Before, the gain jumped
        // down in one sample and released after 50 ms, which distorted and pumped the bass.
        if (targetGain <= mHeldTarget) {
            mHeldTarget = targetGain;
            mHoldLeft = mLookaheadSamples;
        } else if (mHoldLeft > 0) {
            mHoldLeft--;
        } else {
            mHeldTarget = targetGain;
        }
        if (mHeldTarget < mGain) {
            mGain = mAttackCoeff * mGain + (1.0f - mAttackCoeff) * mHeldTarget;
        } else {
            mGain = mReleaseCoeff * mGain + (1.0f - mReleaseCoeff) * mHeldTarget;
        }

        float outL = mDelayL[mDelayPos] * mGain;
        float outR = mDelayR[mDelayPos] * mGain;
        // The smooth attack can leave a sliver of a sudden peak over the threshold.
        outL = std::clamp(outL, -mThreshold, mThreshold);
        outR = std::clamp(outR, -mThreshold, mThreshold);

        mDelayL[mDelayPos] = inL;
        mDelayR[mDelayPos] = inR;
        mDelayPos = (mDelayPos + 1) % mLookaheadSamples;

        buffer[f * 2] = outL;
        buffer[f * 2 + 1] = outR;
    }
}

void Limiter::setEnabled(bool enabled) {
    mEnabled = enabled;
}

void Limiter::reset() {
    std::memset(mDelayL, 0, sizeof(mDelayL));
    std::memset(mDelayR, 0, sizeof(mDelayR));
    mDelayPos = 0;
    mGain = 1.0f;
    mHeldTarget = 1.0f;
    mHoldLeft = 0;
}

}  // namespace axionfx
