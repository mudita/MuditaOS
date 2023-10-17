// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051Trng.hpp"
#include <log/log.hpp>

namespace bsp
{
    namespace
    {
        constexpr auto trngMaxFrequencyCountLimit = 0x3FFFE;
    }

    RT1051Trng::result_type RT1051Trng::generateRandomNumber()
    {
        status_t status;
        trng_config_t trngConfig{};
        RT1051Trng::result_type result;

        /* Default config is very slow, but creates entropy of very high quality */
        status = TRNG_GetDefaultConfig(&trngConfig);
        if (status != kStatus_Success) {
            LOG_FATAL("Failed to get TRNG default config, error: %" PRIi32, status);
            return max();
        }

        /* Use von Neumann sampling for better randomness */
        trngConfig.sampleMode = kTRNG_SampleModeVonNeumann;
        /* Default value is too small what causes TRNG to fail often */
        trngConfig.frequencyCountLimit.maximum = trngMaxFrequencyCountLimit;

        status = TRNG_Init(TRNG, &trngConfig);
        if (status != kStatus_Success) {
            LOG_FATAL("Failed to initialize TRNG module, error: %" PRIi32, status);
            return max();
        }

        status = TRNG_GetRandomData(TRNG, static_cast<void *>(&result), sizeof(result));
        if (status != kStatus_Success) {
            LOG_FATAL("Failed to get random value, error: %" PRIi32, status);
            return max();
        }

        TRNG_Deinit(TRNG);

        return result;
    }
}; // namespace bsp
