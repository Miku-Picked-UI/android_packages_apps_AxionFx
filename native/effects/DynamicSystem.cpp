/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

#include "DynamicSystem.h"

#include <algorithm>
#include <cstdint>

namespace axionfx {

namespace {
// ViPER Dynamic System settings per AxionFx profile. These values are Miku-Picked UI's own
// tuning (measured offline for a smooth lift), not ViPER4Android's device presets:
//  xLow / xHigh  band split: the bass below xLow is processed (above 120 Hz ViPER uses its
//                full two-stage split)
//  yLow / yHigh  split of that bass into sub bass / core / upper bass
//  sideX / sideY gain of the upper bass and the sub bass relative to the core
//  maxGain       ViPER bass gain at 100 % strength (1 = unchanged)
struct Profile {
    uint32_t xLow, xHigh, yLow, yHigh;
    float sideX, sideY, maxGain;
};
// 0 Earbuds, 1 In-ear monitors, 2 ANC true wireless earbuds, 3 Over-ear headphones,
// 4 Open-back headphones, 5 Phone speaker, 6 External speaker, 7 Custom
constexpr Profile kProfiles[DynamicSystem::kProfileCount] = {
    {300, 6200, 50, 90, 1.0f, 0.8f, 3.0f},
    {200, 6200, 40, 60, 1.0f, 1.0f, 3.0f},
    {250, 6200, 40, 80, 1.0f, 0.8f, 3.0f},
    {200, 6200, 40, 60, 1.0f, 1.0f, 3.0f},
    {300, 6200, 40, 60, 1.0f, 1.0f, 3.0f},
    // a phone speaker cannot play deep bass: lift 150-250 Hz, keep the sub bass low
    {700, 6200, 200, 350, 1.0f, 0.3f, 2.0f},
    {300, 6200, 60, 100, 1.0f, 0.5f, 2.5f},
    {300, 6200, 40, 100, 1.0f, 1.0f, 3.0f},  // Custom: xLow / yHigh set by the user
};
}  // namespace

void DynamicSystem::configure(float sampleRate) {
    mSampleRate = sampleRate;
    mViper.SetSamplingRate(static_cast<uint32_t>(sampleRate));
    mViper.SetEnable(true);  // AxionFx decides whether process() runs
    apply();
    mViper.Reset();
}

void DynamicSystem::apply() {
    Profile p = kProfiles[mProfile];
    if (mProfile == kCustomProfile) {
        p.xLow = static_cast<uint32_t>(mCustomLowMidHz);
        p.yHigh = static_cast<uint32_t>(std::max(mCustomBassHz, static_cast<float>(p.yLow) + 10.0f));
    }
    const uint32_t nyq = static_cast<uint32_t>(mSampleRate * 0.45f);
    mViper.SetXCoeffs(std::min(p.xLow, nyq), std::min(p.xHigh, nyq));
    mViper.SetYCoeffs(std::min(p.yLow, nyq), std::min(p.yHigh, nyq));
    mViper.SetSideGain(p.sideX, p.sideY);
    mViper.SetBassGain(1.0f + (p.maxGain - 1.0f) * mStrength);
}

void DynamicSystem::setProfile(int profile) {
    mProfile = std::clamp(profile, 0, kProfileCount - 1);
    apply();
}

void DynamicSystem::setStrength(float percent) {
    mStrength = std::clamp(percent / 100.0f, 0.0f, 1.0f);
    apply();
}

void DynamicSystem::setCustomBassHz(float hz) {
    mCustomBassHz = std::clamp(hz, 30.0f, 300.0f);
    if (mProfile == kCustomProfile) apply();
}

void DynamicSystem::setCustomLowMidHz(float hz) {
    mCustomLowMidHz = std::clamp(hz, 150.0f, 1000.0f);
    if (mProfile == kCustomProfile) apply();
}

void DynamicSystem::process(float* buffer, int frames) {
    if (!mEnabled || frames <= 0 || mStrength <= 0.0f) return;
    mViper.Process(buffer, static_cast<uint32_t>(frames));
}

void DynamicSystem::reset() {
    mViper.Reset();
}

}  // namespace axionfx
