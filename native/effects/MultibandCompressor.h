/*
 * Copyright 2025-2026 AxionOS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "../dsp/Biquad.h"
#include <cmath>

namespace axionfx {

static constexpr int MCOMP_BANDS = 4;

struct MCompBand {
    float thresholdDb = -20.0f;
    float ratio = 4.0f;
    float attackMs = 5.0f;
    float releaseMs = 50.0f;
    float makeupDb = 0.0f;

    float envelopeDb = -96.0f;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float makeupLinear = 1.0f;
};

class MultibandCompressor {
public:
    void configure(float sampleRate);
    void process(float* buffer, int frames);
    void setEnabled(bool enabled);
    bool isEnabled() const { return mEnabled; }

    void setBandThreshold(int band, float dB);
    void setBandRatio(int band, float ratio);
    void setBandAttack(int band, float ms);
    void setBandRelease(int band, float ms);
    void setBandMakeup(int band, float dB);
    void setCrossoverFreq(int index, float hz);

    void reset();

private:
    static constexpr int MAX_BLOCK_FRAMES = 4096;

    void updateCoeffs(MCompBand& band);
    float compressGain(const MCompBand& band, float inputDb) const;
    void processBlock(float* buffer, int frames);

    bool mEnabled = false;
    float mSampleRate = 48000.0f;

    float mCrossoverFreqs[MCOMP_BANDS - 1] = {200.0f, 1000.0f, 5000.0f};

    // Linkwitz-Riley 4th order split (two Butterworth stages each side): its two halves add
    // back up flat. Plain 2nd order Butterworth halves cancel at the crossover and left a deep
    // notch at every crossover frequency (measured: -6 dB at 200 Hz, -14 dB at 1 kHz,
    // -9 dB at 5 kHz).
    struct Lr4Split {
        Biquad lp1, lp2, hp1, hp2;
        void configure(float sampleRate, float hz);
        void reset();
        inline void split(float in, float& low, float& high) {
            low = lp2.process(lp1.process(in));
            high = hp2.process(hp1.process(in));
        }
        // The low and high halves summed: an all-pass with the split's phase response
        inline float allpass(float in) {
            float lo, hi;
            split(in, lo, hi);
            return lo + hi;
        }
    };

    void configureCrossover(int index);

    // [crossover][channel]
    Lr4Split mSplit[MCOMP_BANDS - 1][2];
    // Phase compensation: band b passes through the all-pass of every higher crossover j > b,
    // so all bands are in phase again when summed. [band][crossover][channel]
    Lr4Split mComp[MCOMP_BANDS - 1][MCOMP_BANDS - 1][2];

    MCompBand mBands[MCOMP_BANDS];

    float mBandBufL[MCOMP_BANDS][MAX_BLOCK_FRAMES];
    float mBandBufR[MCOMP_BANDS][MAX_BLOCK_FRAMES];
};

}  // namespace axionfx
