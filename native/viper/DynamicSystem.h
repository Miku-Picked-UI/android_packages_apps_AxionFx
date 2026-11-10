/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#pragma once

#include <cstdint>
#include "DynamicBass.h"

namespace viper {

class DynamicSystem {
public:
    DynamicSystem();

    void Process(float *samples, uint32_t size);
    void Reset();
    void SetBassGain(float gain);
    void SetEnable(bool enable);
    void SetSamplingRate(uint32_t samplingRate);
    void SetSideGain(float gainX, float gainY);
    void SetXCoeffs(uint32_t low, uint32_t high);
    void SetYCoeffs(uint32_t low, uint32_t high);

private:
    DynamicBass dynamicBass;
    uint32_t samplingRate;
    bool enable;
};

}  // namespace viper
