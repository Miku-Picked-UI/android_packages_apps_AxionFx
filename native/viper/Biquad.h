/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#pragma once

#include <cstdint>

// Iscle: Verified with the latest version at 13/12/2022

namespace viper {

class Biquad {
public:
    Biquad();

    double ProcessSample(double sample);
    void Reset();
    void SetBandPassParameter(float frequency, uint32_t samplingRate, float qFactor);
    void SetCoeffs(double a0, double a1, double a2, double b0, double b1, double b2);
    void SetHighPassParameter(float frequency, uint32_t samplingRate, double dbGain, float qFactor, double param_6);
    void SetLowPassParameter(float frequency, uint32_t samplingRate, float qFactor);

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
