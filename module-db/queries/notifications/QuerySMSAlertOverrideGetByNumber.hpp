// Copyright (c) 2017-2026, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Common/Query.hpp>
#include <PhoneNumber.hpp>

#include <optional>
#include <cstdint>
#include <string>

namespace db::query::notifications
{
    struct SMSAlertOverride
    {
        std::string normalizedNumber;
        std::string soundPath;
        bool ignoreDnd = false;
      bool forceVibration = false;
      bool forceMaxVolume = false;
      std::uint32_t repeatIntervalSec = 30;
        bool enabled   = false;
    };

    class SMSAlertOverrideGetByNumber : public Query
    {
      public:
        explicit SMSAlertOverrideGetByNumber(const utils::PhoneNumber::View &number);

        [[nodiscard]] auto getNumber() const noexcept -> const utils::PhoneNumber::View &;
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        utils::PhoneNumber::View number;
    };

    class SMSAlertOverrideGetByNumberResult : public QueryResult
    {
      public:
        explicit SMSAlertOverrideGetByNumberResult(std::optional<SMSAlertOverride> record);

        [[nodiscard]] auto getResult() const -> const std::optional<SMSAlertOverride> &;
        [[nodiscard]] auto debugInfo() const -> std::string override;

      private:
        std::optional<SMSAlertOverride> record;
    };
} // namespace db::query::notifications