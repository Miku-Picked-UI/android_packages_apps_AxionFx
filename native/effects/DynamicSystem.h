/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 *
 * Dynamic System: ViPER4Android's Dynamic System (ViPERFX_RE by Iscle / AndroidAudioMods, see
 * ../viper) driven by AxionFx's listening device profiles. The profile picks ViPER's band
 * split (X / Y frequencies and side gains), the strength its bass gain.
 */
#pragma once

#include "../viper/DynamicSystem.h"
#include "../viper/MultiBiquad.h"

namespace axionfx {

class DynamicSystem {
public:
    void configure(float sampleRate);
    void process(float* buffer, int frames);
    void setEnabled(bool enabled) { mEnabled = enabled; }
    bool isEnabled() const { return mEnabled; }
    void setProfile(int profile);
    /** While audio plays on the phone speaker the phone speaker profile is always used. */
    void setSpeakerActive(bool active);
    void setStrength(float percent);
    void setCustomBassHz(float hz);
    void setCustomLowMidHz(float hz);
    void reset();

    static constexpr int kProfileCount = 8;  // the last one is Custom
    static constexpr int kCustomProfile = kProfileCount - 1;

private:
    void apply();

    bool mEnabled = false;
    float mSampleRate = 48000.0f;
    int mProfile = 0;
    bool mSpeakerActive = false;
    bool mSubCut = false;               // phone speaker profile: high-pass after ViPER
    viper::MultiBiquad mSubCutFilter[2];
    static constexpr float kSubCutHz = 100.0f;
    static constexpr int kSpeakerProfile = 5;
    float mStrength = 0.5f;  // 0..1
    float mCustomBassHz = 100.0f;
    float mCustomLowMidHz = 300.0f;

    viper::DynamicSystem mViper;
};

}  // namespace axionfx
