/*
 * SPDX-FileCopyrightText: 2026 Miku-Picked UI
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Clarity.h"

#include <algorithm>
#include <cstdint>

namespace axionfx {

void Clarity::configure(float sampleRate) {
    mViper.SetSamplingRate(static_cast<uint32_t>(sampleRate));
    mViper.SetProcessMode(static_cast<viper::ViPERClarity::ClarityMode>(mMode));
    mViper.SetClarity(mAmount * kMaxViperGain);
    // AxionFx gates the effect itself; ViPER's own switch stays on.
    mViper.SetEnable(true);
    mViper.Reset();
}

void Clarity::setEnabled(bool enabled) {
    if (enabled && !mEnabled) mViper.Reset();  // don't carry stale filter state over
    mEnabled = enabled;
}

void Clarity::setMode(int mode) {
    mMode = std::clamp(mode, static_cast<int>(NATURAL), static_cast<int>(XHIFI));
    mViper.SetProcessMode(static_cast<viper::ViPERClarity::ClarityMode>(mMode));
}

void Clarity::setAmount(float percent) {
    mAmount = std::clamp(percent / 100.0f, 0.0f, 1.0f);
    mViper.SetClarity(mAmount * kMaxViperGain);
}

void Clarity::process(float* buffer, int frames) {
    if (!mEnabled || frames <= 0 || mAmount <= 0.0f) return;
    mViper.Process(buffer, static_cast<uint32_t>(frames));
}

void Clarity::reset() {
    mViper.Reset();
}

}  // namespace axionfx
