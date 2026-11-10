/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#pragma once

#include <cstdint>
#include "MultiBiquad.h"
#include "Biquad.h"
#include <array>

// Iscle: Verified with the latest version at 13/12/2022

namespace viper {

class SpeakerCorrection {
public:
    SpeakerCorrection();

    void Process(float *samples, uint32_t size);
    void Reset();
    void SetEnable(bool enable);
    void SetSamplingRate(uint32_t samplingRate);

private:
    uint32_t samplingRate;
    bool enable;
    std::array<MultiBiquad, 2> highPass;
    std::array<Biquad, 2> lowPass;
    std::array<Biquad, 2> bandPass;
};

}  // namespace viper
