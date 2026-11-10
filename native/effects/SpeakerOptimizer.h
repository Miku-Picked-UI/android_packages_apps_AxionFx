/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "../viper/SpeakerCorrection.h"

namespace axionfx {

/**
 * Speaker optimization: ViPER4Android's speaker correction (ViPERFX_RE by Iscle /
 * AndroidAudioMods, see ../viper) for the built-in phone speaker. The amount blends the
 * corrected signal with the original (100 % = ViPER's correction exactly).
 *
 * It only processes while the output is the phone speaker (setSpeakerActive).
 */
class SpeakerOptimizer {
public:
    void configure(float sampleRate);
    void process(float* buffer, int frames);
    void setEnabled(bool enabled) { mEnabled = enabled; }
    bool isEnabled() const { return mEnabled; }
    void setSpeakerActive(bool active) { mSpeakerActive = active; }
    void setAmount(float percent);
    void reset();

private:
    bool mEnabled = false;
    bool mSpeakerActive = true;
    float mAmount = 0.6f;  // 0..1

    viper::SpeakerCorrection mViper;
    static constexpr int kChunk = 256;  // frames per pass (dry copy on the stack)
};

}  // namespace axionfx
