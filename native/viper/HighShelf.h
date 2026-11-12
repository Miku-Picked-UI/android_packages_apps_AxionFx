/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#pragma once

#include <cstdint>

namespace viper {

class HighShelf {
public:
    double Process(double sample);
    void SetFrequency(float freq);
    void SetGain(float gain);
    void SetSamplingRate(uint32_t samplingRate);

private:
    float frequency;
    double gain;
    double x_1;
    double x_2;
    double y_1;
    double y_2;
    double b0;
    double b1;
    double b2;
    double a0;
    double a1;
    double a2;
};

}  // namespace viper
