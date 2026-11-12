/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#include "IIR_NOrder_BW_LH.h"

namespace viper {

IIR_NOrder_BW_LH::IIR_NOrder_BW_LH(uint32_t order) {
    this->filters = std::vector<IIR_1st>(order);
    this->order = order;

    for (uint32_t x = 0; x < order; x++) {
        this->filters[x].Mute();
    }
}

void IIR_NOrder_BW_LH::Mute() {
    for (uint32_t x = 0; x < this->order; x++) {
        this->filters[x].Mute();
    }
}

void IIR_NOrder_BW_LH::setLPF(float frequency, uint32_t samplingRate) {
    for (uint32_t x = 0; x < this->order; x++) {
        this->filters[x].setLPF_BW(frequency, samplingRate);
    }
}

void IIR_NOrder_BW_LH::setHPF(float frequency, uint32_t samplingRate) {
    for (uint32_t x = 0; x < this->order; x++) {
        this->filters[x].setHPF_BW(frequency, samplingRate);
    }
}

}  // namespace viper
