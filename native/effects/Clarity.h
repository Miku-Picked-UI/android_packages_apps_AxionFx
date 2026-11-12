/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 *
 * Clarity: ViPER4Android's Clarity (ViPERFX_RE by Iscle / AndroidAudioMods, see ../viper),
 * driven by AxionFx's mode and amount controls.
 *  - NATURAL: ViPER's noise sharpening.
 *  - OZONE:   ViPER's OZone+ high shelf at 8.25 kHz.
 *  - XHIFI:   ViPER's X-HiFi band split with delayed, boosted highs.
 * The amount (0-100 %) spans ViPER's clarity slider (0-450, a gain of 0-4.5).
 */
#pragma once

#include "../viper/ViPERClarity.h"

namespace axionfx {

class Clarity {
public:
    enum Mode { NATURAL = 0, OZONE = 1, XHIFI = 2 };

    void configure(float sampleRate);
    void process(float* buffer, int frames);
    void setEnabled(bool enabled);
    bool isEnabled() const { return mEnabled; }
    void setMode(int mode);
    void setAmount(float percent);
    void reset();

private:
    static constexpr float kMaxViperGain = 4.5f;  // ViPER4Android's slider maximum (450 / 100)

    bool mEnabled = false;
    int mMode = NATURAL;
    float mAmount = 0.5f;  // 0..1

    viper::ViPERClarity mViper;
};

}  // namespace axionfx
