// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-bsp/hal/temperature_source/TemperatureSource.hpp>

namespace evtmgr::internal
{
    class StaticData
    {
      public:
        StaticData(const StaticData &) = delete;
        StaticData &operator=(const StaticData &) = delete;

        static StaticData &get();

        void setCurrentTemperature(hal::temperature::AbstractTemperatureSource::Temperature temperature);
        [[nodiscard]] hal::temperature::AbstractTemperatureSource::Temperature getCurrentTemperature() const;

      private:
        StaticData() = default;

        hal::temperature::AbstractTemperatureSource::Temperature currentTemperature;
    };
} // namespace evtmgr::internal
