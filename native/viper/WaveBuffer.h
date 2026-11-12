/*
 * From ViPERFX_RE (https://github.com/AndroidAudioMods/ViPERFX_RE, fork
 * https://github.com/WSTxda/ViPERFX_RE), the reverse engineered ViPER4Android FX effect by
 * Iscle / AndroidAudioMods, used in Miku-Picked UI's AxionFx with their permission.
 * Changes: flat includes, wrapped in namespace viper.
 */
#pragma once

#include <cstdint>
#include <vector>

namespace viper {

class WaveBuffer {
public:
    WaveBuffer(uint32_t channels, uint32_t length);

    void Reset();
    uint32_t GetBufferOffset();
    uint32_t GetBufferSize();
    float *GetBuffer();
    uint32_t PopSamples(uint32_t size, bool resetIndex);
    uint32_t PopSamples(float *dest, uint32_t size, bool resetIndex);
    int PushSamples(float *source, uint32_t size);
    int PushZeros(uint32_t size);
    float *PushZerosGetBuffer(uint32_t size);
    void SetBufferOffset(uint32_t offset);

private:
    std::vector<float> buffer;
    uint32_t index;
    uint32_t channels;
};

}  // namespace viper
