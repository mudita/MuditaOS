// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-time/api/TimeSettingsApi.hpp>

namespace app
{
    class AbstractTimeModel
    {
      public:
        virtual ~AbstractTimeModel() noexcept = default;

        virtual utils::time::Locale::TimeFormat getTimeFormat() const = 0;
        virtual time_t getCurrentTime() const                         = 0;
    };

    class TimeModel : public AbstractTimeModel
    {
      public:
        utils::time::Locale::TimeFormat getTimeFormat() const override;
        time_t getCurrentTime() const override;
    };
} // namespace app
