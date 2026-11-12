/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#include "IIR_NOrder_BW_BP.h"

namespace viper {

IIR_NOrder_BW_BP::IIR_NOrder_BW_BP(uint32_t order) {
    this->lowpass = std::vector<IIR_1st>(order);
    this->highpass = std::vector<IIR_1st>(order);
    this->order = order;

    for (uint32_t x = 0; x < order; x++) {
        this->lowpass[x].Mute();
        this->highpass[x].Mute();
    }
}

void IIR_NOrder_BW_BP::Mute() {
    for (uint32_t x = 0; x < this->order; x++) {
        this->lowpass[x].Mute();
        this->highpass[x].Mute();
    }
}

void IIR_NOrder_BW_BP::setBPF(float highCut, float lowCut, uint32_t samplingRate) {
    for (uint32_t x = 0; x < this->order; x++) {
        this->lowpass[x].setLPF_BW(lowCut, samplingRate);
        this->highpass[x].setHPF_BW(highCut, samplingRate);
    }
}

}  // namespace viper
