// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Urc.hpp"
#include <ctime>

namespace at::urc
{
    class Ctze : public Urc
    {
        enum Tokens
        {
            GMTDifference,
            DaylightSavingsAdjustment,
            Date,
            Time
        };

      public:
        static constexpr std::string_view head = "+CTZE";
        static auto isURC(const std::string &uHead) -> bool
        {
            return uHead.find(Ctze::head) != std::string::npos;
        }

        using Urc::Urc;

        constexpr static int maxTimezoneOffset = 56;
        constexpr static int minTimezoneOffset = -48;

        [[nodiscard]] auto isValid() const noexcept -> bool override;
        [[nodiscard]] auto getTimeInfo() const noexcept -> tm;

        [[nodiscard]] auto getTimeZoneOffset() const -> int;
        [[nodiscard]] auto getTimeZoneString() const -> std::string;
        [[nodiscard]] auto getGMTTime() const -> const struct tm;

        void Handle(UrcHandler &h) final
        {
            h.Handle(*this);
        }
    };
} // namespace at::urc
