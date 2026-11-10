/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#pragma once

#include <cstdint>

namespace viper {

class MultiBiquad {
public:
    enum FilterType {
        LOW_PASS,
        HIGH_PASS,
        BAND_PASS,
        BAND_STOP,
        ALL_PASS,
        PEAK,
        LOW_SHELF,
        HIGH_SHELF
    };

    MultiBiquad();

    double ProcessSample(double sample);
    void RefreshFilter(FilterType type, float gainAmp, float frequency, uint32_t samplingRate, float qFactor, bool param_7);

private:
    double x1;
    double x2;
    double y1;
    double y2;
    double a1;
    double a2;
    double b0;
    double b1;
    double b2;
};

}  // namespace viper
