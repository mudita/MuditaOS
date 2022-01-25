// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Equalizer.hpp"

#include <cstdint>
#include <cmath>
#include <stdexcept>

namespace audio::equalizer
{
    QFilterCoefficients qfilter_CalculateCoeffs(
        FilterType filter, float frequency, uint32_t samplerate, float Q, float gain)
    {
        constexpr auto qMinValue         = .1f;
        constexpr auto qMaxValue         = 10.f;
        constexpr auto frequencyMinValue = 0.f;
        if (frequency < frequencyMinValue && filter != FilterType::None) {
            throw std::invalid_argument("Negative frequency provided");
        }
        if ((Q < qMinValue || Q > qMaxValue) && filter != FilterType::None) {
            throw std::invalid_argument("Q out of range");
        }
        QFilterCoefficients filter_coeff;
        float a0       = 0;
        float omega    = 2 * M_PI * frequency / samplerate;
        float sn       = sin(omega);
        float cs       = cos(omega);
        float alpha    = sn / (2 * Q);
        float gain_abs = pow(10, gain / 40);

        switch (filter) {
        case FilterType::BandPass:
            filter_coeff.b0 = alpha;
            filter_coeff.b1 = 0;
            filter_coeff.b2 = -alpha;
            filter_coeff.a1 = -2 * cs;
            filter_coeff.a2 = 1 - alpha;
            a0              = 1 + alpha;
            break;

        case FilterType::HighPass:
            filter_coeff.b0 = (1 + cs) / 2;
            filter_coeff.b1 = -(1 + cs);
            filter_coeff.b2 = (1 + cs) / 2;
            filter_coeff.a1 = -2 * cs;
            filter_coeff.a2 = 1 - alpha;
            a0              = 1 + alpha;
            break;

        case FilterType::LowPass:
            filter_coeff.b0 = (1 - cs) / 2;
            filter_coeff.b1 = 1 - cs;
            filter_coeff.b2 = (1 - cs) / 2;
            filter_coeff.a1 = -2 * cs;
            filter_coeff.a2 = 1 - alpha;
            a0              = 1 + alpha;
            break;
        case FilterType::Flat:
            filter_coeff.b0 = 0.0;
            filter_coeff.b1 = 0.0;
            filter_coeff.b2 = 1;
            filter_coeff.a1 = 0.0;
            filter_coeff.a2 = 0.0;
            a0              = 1;
            break;
        case FilterType::Notch:
            filter_coeff.b0 = 1;
            filter_coeff.b1 = -2 * cs;
            filter_coeff.b2 = 1;
            filter_coeff.a1 = -2 * cs;
            filter_coeff.a2 = 1 - alpha;
            a0              = 1 + alpha;
            break;
        case FilterType::HighShelf:
            filter_coeff.b0 = gain_abs * ((gain_abs + 1) + (gain_abs - 1) * cs + 2 * sqrt(gain_abs) * alpha);
            filter_coeff.b1 = -2 * gain_abs * ((gain_abs - 1.0) + (gain_abs + 1) * cs);
            filter_coeff.b2 = gain_abs * ((gain_abs + 1) + (gain_abs - 1) * cs - 2 * sqrt(gain_abs) * alpha);
            a0              = (gain_abs + 1) - (gain_abs - 1) * cs + 2 * sqrt(gain_abs) * alpha;
            filter_coeff.a1 = 2 * ((gain_abs - 1) - (gain_abs + 1) * cs);
            filter_coeff.a2 = (gain_abs + 1) - (gain_abs - 1) * cs - 2 * sqrt(gain_abs) * alpha;
            break;
        case FilterType::LowShelf:
            filter_coeff.b0 = gain_abs * ((gain_abs + 1) - (gain_abs - 1) * cs + 2 * sqrt(gain_abs) * alpha);
            filter_coeff.b1 = 2 * gain_abs * ((gain_abs - 1.0) - (gain_abs + 1) * cs);
            filter_coeff.b2 = gain_abs * ((gain_abs + 1) - (gain_abs - 1) * cs - 2 * sqrt(gain_abs) * alpha);
            a0              = (gain_abs + 1) + (gain_abs - 1) * cs + 2 * sqrt(gain_abs) * alpha;
            filter_coeff.a1 = -2 * ((gain_abs - 1) + (gain_abs + 1) * cs);
            filter_coeff.a2 = (gain_abs + 1) + (gain_abs - 1) * cs - 2 * sqrt(gain_abs) * alpha;
            break;
        case FilterType::Parametric:
            filter_coeff.b0 = 1.0 + alpha * gain_abs;
            filter_coeff.b1 = -2.0 * cs;
            filter_coeff.b2 = 1.0 - alpha * gain_abs;
            a0              = 1.0 + alpha / gain_abs;
            filter_coeff.a1 = -2.0 * cs;
            filter_coeff.a2 = 1.0 - alpha / gain_abs;
            break;
        case FilterType::None:
            filter_coeff.b0 = 1;
            filter_coeff.b1 = 0;
            filter_coeff.b2 = 0;
            a0              = 1;
            filter_coeff.a1 = 0;
            filter_coeff.a2 = 0;
            break;
        }

        // prescale flter constants
        filter_coeff.b0 /= a0;
        filter_coeff.b1 /= a0;
        filter_coeff.b2 /= a0;
        filter_coeff.a1 /= a0;
        filter_coeff.a2 /= a0;

        return filter_coeff;
    }
} // namespace audio::equalizer
