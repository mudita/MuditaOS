// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "URC.hpp"
#include <ctime>

namespace at::urc
{
    class CTZE : public URC
    {
        enum Tokens
        {
            GMTDifference,
            DaylightSavingsAdjustment,
            Date,
            Time
        };

      public:
        inline static const std::string head = "+CTZE";
        static bool isURC(const std::string uHead)
        {
            return uHead.find(CTZE::head) != std::string::npos;
        }

        using URC::URC;

        constexpr static int maxTimezoneOffset = 56;
        constexpr static int minTimezoneOffset = -48;

        [[nodiscard]] bool isValid() const noexcept override;
        [[nodiscard]] auto getTimeInfo() const noexcept -> tm;

        [[nodiscard]] auto getTimeZoneOffset() const -> int;
        [[nodiscard]] auto getTimeZoneString() const -> std::string;
        [[nodiscard]] auto getGMTTime() const -> const struct tm;

        void Handle(URCHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
