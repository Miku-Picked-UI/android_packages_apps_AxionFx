/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#pragma once


#include "IIR_NOrder_BW_LH.h"
#include "WaveBuffer.h"
#include "IIR_NOrder_BW_BP.h"

namespace viper {

class HiFi {
public:
    HiFi();

    ~HiFi();

    void Process(float *samples, uint32_t size);

    void Reset();

    void SetClarity(float value);

    void SetSamplingRate(uint32_t samplingRate);

    WaveBuffer *buffers[2];
    struct {
        IIR_NOrder_BW_LH *lowpass;
        IIR_NOrder_BW_LH *highpass;
        IIR_NOrder_BW_BP *bandpass;
    } filters[2];
    float gain;
    uint32_t samplingRate;
};

}  // namespace viper
