/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#pragma once

#include <cstdint>
#include "PolesFilter.h"
#include "Biquad.h"

namespace viper {

class DynamicBass {
public:
    DynamicBass();

    void FilterSamples(float *samples, uint32_t size);

    void Reset();

    void SetBassGain(float gain);

    void SetFilterXPassFrequency(uint32_t low, uint32_t high);

    void SetFilterYPassFrequency(uint32_t low, uint32_t high);

    void SetSamplingRate(uint32_t samplingRate);

    void SetSideGain(float gainX, float gainY);

    uint32_t lowFreqX, highFreqX;
    uint32_t lowFreqY, highFreqY;
    uint32_t samplingRate;
    float qPeak;
    float bassGain;
    float sideGainX, sideGainY;

    PolesFilter filterX;
    PolesFilter filterY;
    Biquad lowPass;
};

}  // namespace viper
