#pragma once

#include "URC_Any.hpp"
#include <ctime>

namespace at::urc
{
    struct CTZE : public Any
    {
        const std::string urc_name = "+CTZE";

        CTZE(const std::string &val);
        ~CTZE() override = default;
        auto what() -> std::string final;

        enum class tokensDescription
        {
            GMTdiference,
            daylightSavingsAdjustment,
            date,
            time
        };
        struct tm getTimeInfo(void);
    };
}; // namespace at::urc
