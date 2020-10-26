// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC_Any.hpp"
#include <ctime>

namespace at::urc
{
    class CTZE : public Any
    {
        const std::string urc_name = "+CTZE";
        enum Tokens
        {
            GMTDifference,
            DaylightSavingsAdjustment,
            Date,
            Time
        };

      public:
        explicit CTZE(const std::string &val);

        constexpr static int maxTimezoneOffset = 56;
        constexpr static int minTimezoneOffset = -48;

        ~CTZE() override = default;
        [[nodiscard]] auto what() const noexcept -> std::string final;
        [[nodiscard]] auto isValid() const noexcept -> bool;
        [[nodiscard]] auto getTimeInfo(void) const noexcept -> tm;

        auto getTimeZoneOffset() const -> int;
        auto getTimeZoneString() const -> const std::string;
        auto getGMTTime(void) const -> const struct tm;
    };
}; // namespace at::urc
