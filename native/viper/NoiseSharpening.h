/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#pragma once


#include "IIR_1st.h"

namespace viper {

class NoiseSharpening {
public:
    NoiseSharpening();

    void Process(float *buffer, uint32_t size);

    void Reset();

    void SetGain(float gain);

    void SetSamplingRate(uint32_t samplingRate);

    IIR_1st filters[2];
    float in[2];
    uint32_t samplingRate;
    float gain;
};

}  // namespace viper
