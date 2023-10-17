// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "qfilter.h"

#include <stdint.h>
#include <math.h>

int qfilter_CalculateCoeffs(qfilter_t filter,
                            float frequency,
                            uint32_t samplerate,
                            float Q,
                            float gain,
                            qfilter_coefficients_t *const filter_coeff)
{
    float a0       = 0;
    float omega    = 2 * M_PI * frequency / samplerate;
    float sn       = sin(omega);
    float cs       = cos(omega);
    float alpha    = sn / (2 * Q);
    float gain_abs = pow(10, gain / 40);

    switch (filter) {
    case FilterBandPass:
        filter_coeff->b0 = alpha;
        filter_coeff->b1 = 0;
        filter_coeff->b2 = -alpha;
        filter_coeff->a1 = -2 * cs;
        filter_coeff->a2 = 1 - alpha;
        a0               = 1 + alpha;
        break;

    case FilterHighPass:
        filter_coeff->b0 = (1 + cs) / 2;
        filter_coeff->b1 = -(1 + cs);
        filter_coeff->b2 = (1 + cs) / 2;
        filter_coeff->a1 = -2 * cs;
        filter_coeff->a2 = 1 - alpha;
        a0               = 1 + alpha;
        break;

    case FilterLowPass:
        filter_coeff->b0 = (1 - cs) / 2;
        filter_coeff->b1 = 1 - cs;
        filter_coeff->b2 = (1 - cs) / 2;
        filter_coeff->a1 = -2 * cs;
        filter_coeff->a2 = 1 - alpha;
        a0               = 1 + alpha;
        break;
    case FilterFlat:
        filter_coeff->b0 = 0.0;
        filter_coeff->b1 = 0.0;
        filter_coeff->b2 = 1;
        filter_coeff->a1 = 0.0;
        filter_coeff->a2 = 0.0;
        a0               = 1;
        break;
    case FilterNotch:
        filter_coeff->b0 = 1;
        filter_coeff->b1 = -2 * cs;
        filter_coeff->b2 = 1;
        filter_coeff->a1 = -2 * cs;
        filter_coeff->a2 = 1 - alpha;
        a0               = 1 + alpha;
        break;

    case FilterPeak:
        filter_coeff->b0 = 1 + (alpha * gain_abs);
        filter_coeff->b1 = -2 * cs;
        filter_coeff->b2 = 1 - (alpha * gain_abs);
        filter_coeff->a1 = -2 * cs;
        filter_coeff->a2 = 1 - (alpha / gain_abs);
        a0               = 1 + (alpha / gain_abs);
        break;
    default:
        return 0;
    }

    // prescale flter constants
    filter_coeff->b0 /= a0;
    filter_coeff->b1 /= a0;
    filter_coeff->b2 /= a0;
    filter_coeff->a1 /= a0;
    filter_coeff->a2 /= a0;

    return 1;
}
