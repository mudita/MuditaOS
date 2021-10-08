// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    typedef struct
    {
        float b0;
        float b1;
        float b2;
        float a1;
        float a2;
    } qfilter_coefficients_t;

    typedef enum
    {
        FilterBandPass,
        FilterHighPass,
        FilterLowPass,
        FilterFlat,
        FilterNotch,
        FilterPeak,
    } qfilter_t;

    int qfilter_CalculateCoeffs(qfilter_t filter,
                                float frequency,
                                uint32_t samplerate,
                                float Q,
                                float gain,
                                qfilter_coefficients_t *const filter_coeff);

#ifdef __cplusplus
}
#endif
