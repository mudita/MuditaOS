// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <service-time/api/TimeSettingsApi.hpp>

namespace app
{
    class AbstractTimeModel
    {
      public:
        virtual ~AbstractTimeModel() noexcept = default;

        [[nodiscard]] virtual utils::time::Locale::TimeFormat getTimeFormat() const = 0;
        [[nodiscard]] virtual std::time_t getCurrentTime() const                    = 0;
    };

    class TimeModel : public AbstractTimeModel
    {
      public:
        [[nodiscard]] utils::time::Locale::TimeFormat getTimeFormat() const override;
        [[nodiscard]] std::time_t getCurrentTime() const override;
    };
} // namespace app
