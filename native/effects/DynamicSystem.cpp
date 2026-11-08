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
constexpr float kQuietLevel = 0.02f;  // bass envelope where the full boost applies
constexpr float kLoudLevel = 0.30f;   // bass envelope where the boost has faded out
}  // namespace

void DynamicSystem::configure(float sampleRate) {
    mSampleRate = sampleRate;
    mEnvelope.configure(sampleRate, 10.0f, 180.0f);
    mGainSmoothing = 1.0f - std::exp(-1.0f / (0.02f * sampleRate));  // ~20 ms
    updateFilters();
}

void DynamicSystem::updateFilters() {
    Profile p = kProfiles[mProfile];
    if (mProfile == kCustomProfile) {
        p.bassHz = mCustomBassHz;
        p.lowMidHz = mCustomLowMidHz;
    }
    const float nyq = mSampleRate * 0.45f;
    p.bassHz = std::min(p.bassHz, nyq);
    p.lowMidHz = std::min(p.lowMidHz, nyq);
    for (int c = 0; c < 2; ++c) {
        mLow1[c].configure(BiquadType::LOWPASS, mSampleRate, p.bassHz, 0.0f, 0.707f);
        mLow2[c].configure(BiquadType::LOWPASS, mSampleRate, p.bassHz, 0.0f, 0.707f);
        mLowMidCut[c].configure(BiquadType::PEAKING, mSampleRate, p.lowMidHz,
                                -2.5f * mStrength, 1.0f);
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

    const float maxBoostDb = kProfiles[mProfile].maxBoostDb * mStrength;

    for (int i = 0; i < frames; ++i) {
        float lowL = mLow2[0].process(mLow1[0].process(buffer[i * 2]));
        float lowR = mLow2[1].process(mLow1[1].process(buffer[i * 2 + 1]));

        // One envelope for both channels keeps the stereo image stable.
        float env = mEnvelope.process(std::max(std::fabs(lowL), std::fabs(lowR)));
        float t = std::clamp((env - kQuietLevel) / (kLoudLevel - kQuietLevel), 0.0f, 1.0f);
        float target = std::pow(10.0f, maxBoostDb * (1.0f - t) / 20.0f);
        mGain += (target - mGain) * mGainSmoothing;

        float extra = mGain - 1.0f;
        buffer[i * 2] = mLowMidCut[0].process(buffer[i * 2] + lowL * extra);
        buffer[i * 2 + 1] = mLowMidCut[1].process(buffer[i * 2 + 1] + lowR * extra);
    }
}

void DynamicSystem::reset() {
    for (int c = 0; c < 2; ++c) {
        mLow1[c].reset();
        mLow2[c].reset();
        mLowMidCut[c].reset();
    }
    mEnvelope.reset();
    mGain = 1.0f;
}

}  // namespace axionfx
