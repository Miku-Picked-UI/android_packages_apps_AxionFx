/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#pragma once

#include <cstdint>
#include "NoiseSharpening.h"
#include "HiFi.h"
#include "HighShelf.h"

namespace viper {

// Iscle: Verified with the latest version at 13/12/2022

class ViPERClarity {
public:
    enum ClarityMode {
        NATURAL,
        OZONE,
        XHIFI
    };

    ViPERClarity();

    void Process(float *samples, uint32_t size);
    void Reset();
    void SetClarity(float gainPercent);
    void SetClarityToFilter();
    void SetEnable(bool enable);
    void SetProcessMode(ClarityMode processMode);
    void SetSamplingRate(uint32_t samplingRate);

private:
    NoiseSharpening noiseSharpening;
    HighShelf highShelf[2];
    HiFi hifi;
    bool enable;
    ClarityMode processMode;
    uint32_t samplingRate;
    float clarityGainPercent;
};

}  // namespace viper
