/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

#include "DynamicSystem.h"

#include <algorithm>
#include <cmath>

namespace axionfx {

namespace {
struct Profile {
    float bassHz;    // upper edge of the enhanced bass band
    float lowMidHz;  // centre of the tightening cut
    float maxBoostDb;
};
// 0 Earbuds, 1 In-ear monitors, 2 ANC true wireless earbuds, 3 Over-ear headphones,
// 4 Open-back headphones, 5 Phone speaker, 6 External speaker,
// 7 Custom (frequencies set by the user)
constexpr Profile kProfiles[DynamicSystem::kProfileCount] = {
    {110.0f, 320.0f, 12.0f},
    {90.0f, 280.0f, 10.0f},
    // ANC true wireless earbuds (e.g. Galaxy Buds Pro, AirPods Pro, WF-1000XM): small drivers
    // in a sealed fit with their own bass tuning; lift the deep bass that small drivers
    // lack, keep the boost moderate so it stays clean over Bluetooth codecs, and cut a bit
    // of low-mid where these fits tend to boom.
    {75.0f, 230.0f, 8.0f},
    {80.0f, 250.0f, 9.0f},
    {70.0f, 220.0f, 11.0f},
    {180.0f, 450.0f, 6.0f},
    {100.0f, 300.0f, 8.0f},
    {100.0f, 300.0f, 10.0f},
};
// The boosted bass band may reach this peak level; above it the boost is pulled back so
// the bass does not clip or swamp the rest of the mix. Below it the full boost applies.
constexpr float kBassHeadroom = 0.6f;

// Boost in dB for a profile at the given strength (0..1). Even low strengths give an
// audible lift; 100 % gives the profile's full boost.
float boostDb(float maxBoostDb, float strength) {
    return strength <= 0.0f ? 0.0f : maxBoostDb * (0.4f + 0.6f * strength);
}
}  // namespace

void DynamicSystem::configure(float sampleRate) {
    mSampleRate = sampleRate;
    mEnvelope.configure(sampleRate, 10.0f, 180.0f);
    // ~30 ms smoothing, applied once per block of kBlock frames
    mGainSmoothing = 1.0f - std::exp(-static_cast<float>(kBlock) / (0.03f * sampleRate));
    updateFilters();
}

float DynamicSystem::bassHz() const {
    float hz = mProfile == kCustomProfile ? mCustomBassHz : kProfiles[mProfile].bassHz;
    return std::min(hz, mSampleRate * 0.45f);
}

void DynamicSystem::updateFilters() {
    float lowMidHz = mProfile == kCustomProfile ? mCustomLowMidHz : kProfiles[mProfile].lowMidHz;
    lowMidHz = std::min(lowMidHz, mSampleRate * 0.45f);
    const float bass = bassHz();
    for (int c = 0; c < 2; ++c) {
        mLow1[c].configure(BiquadType::LOWPASS, mSampleRate, bass, 0.0f, 0.707f);
        mLow2[c].configure(BiquadType::LOWPASS, mSampleRate, bass, 0.0f, 0.707f);
        mLowMidCut[c].configure(BiquadType::PEAKING, mSampleRate, lowMidHz,
                                -4.0f * mStrength, 0.9f);
    }
    mShelfDb = -1.0f;  // force the shelves to be recomputed for the new frequency
    updateShelves(mGainDb);
}

void DynamicSystem::updateShelves(float gainDb) {
    if (std::fabs(gainDb - mShelfDb) < 0.05f) return;
    mShelfDb = gainDb;
    for (int c = 0; c < 2; ++c) {
        mShelf[c].configure(BiquadType::LOW_SHELF, mSampleRate, bassHz(), gainDb, 0.707f);
    }
}

void DynamicSystem::setProfile(int profile) {
    mProfile = std::clamp(profile, 0, kProfileCount - 1);
    updateFilters();
}

void DynamicSystem::setStrength(float percent) {
    mStrength = std::clamp(percent / 100.0f, 0.0f, 1.0f);
    updateFilters();
}

void DynamicSystem::setCustomBassHz(float hz) {
    mCustomBassHz = std::clamp(hz, 30.0f, 300.0f);
    if (mProfile == kCustomProfile) updateFilters();
}

void DynamicSystem::setCustomLowMidHz(float hz) {
    mCustomLowMidHz = std::clamp(hz, 150.0f, 1000.0f);
    if (mProfile == kCustomProfile) updateFilters();
}

void DynamicSystem::process(float* buffer, int frames) {
    if (!mEnabled || frames <= 0 || mStrength <= 0.0f) return;

    const float maxDb = boostDb(kProfiles[mProfile].maxBoostDb, mStrength);
    const float maxGain = std::pow(10.0f, maxDb / 20.0f);

    for (int i = 0; i < frames; ++i) {
        // Side chain: one envelope of the bass band for both channels keeps the stereo
        // image stable.
        float lowL = mLow2[0].process(mLow1[0].process(buffer[i * 2]));
        float lowR = mLow2[1].process(mLow1[1].process(buffer[i * 2 + 1]));
        float env = mEnvelope.process(std::max(std::fabs(lowL), std::fabs(lowR)));

        if (++mBlockPos >= kBlock) {
            mBlockPos = 0;
            // The full boost applies until the boosted bass would pass the headroom level;
            // then the boost follows the music down (the "dynamic" part), never below 0 dB.
            float targetDb = maxDb;
            if (env * maxGain > kBassHeadroom) {
                targetDb = std::max(0.0f, 20.0f * std::log10(kBassHeadroom / std::max(env, 1e-6f)));
            }
            mGainDb += (targetDb - mGainDb) * mGainSmoothing;
            updateShelves(mGainDb);
        }

        buffer[i * 2] = mLowMidCut[0].process(mShelf[0].process(buffer[i * 2]));
        buffer[i * 2 + 1] = mLowMidCut[1].process(mShelf[1].process(buffer[i * 2 + 1]));
    }
}

void DynamicSystem::reset() {
    for (int c = 0; c < 2; ++c) {
        mLow1[c].reset();
        mLow2[c].reset();
        mLowMidCut[c].reset();
        mShelf[c].reset();
    }
    mEnvelope.reset();
    mGainDb = 0.0f;
    mBlockPos = 0;
    mShelfDb = -1.0f;
    updateShelves(0.0f);
}

}  // namespace axionfx
