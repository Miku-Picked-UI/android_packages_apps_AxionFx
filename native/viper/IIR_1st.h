/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#pragma once

#include <cstdint>

namespace viper {

class IIR_1st {
public:
    IIR_1st();

    void Mute();
    void SetCoefficients(float b0, float b1, float a1);
    void setHPF_A(float frequency, uint32_t samplingRate);
    void setHPF_BW(float frequency, uint32_t samplingRate);
    void setHPF_C(float frequency, uint32_t samplingRate);
    void setHPFwLPS_A(float frequency, uint32_t samplingRate);
    void setHSF_A(float f1, float f2, uint32_t samplingRate);
    void setLPF_A(float frequency, uint32_t samplingRate);
    void setLPF_BW(float frequency, uint32_t samplingRate);
    void setLPF_C(float frequency, uint32_t samplingRate);
    void setLSF_A(float f1, float f2, uint32_t samplingRate);
    void setPole(float a1);
    void setPoleHPF(float frequency, uint32_t samplingRate);
    void setPoleLPF(float frequency, uint32_t samplingRate);
    void setZero(float b1);
    void setZeroHPF(float frequency, uint32_t samplingRate);
    void setZeroLPF(float frequency, uint32_t samplingRate);

    float b0, b1, a1;
    float prevSample;
};

inline float do_filter(IIR_1st *filter, float sample) {
    float hist = sample * filter->b1;
    sample = filter->prevSample + sample * filter->b0;
    filter->prevSample = sample * filter->a1 + hist;
    return sample;
}

}  // namespace viper
