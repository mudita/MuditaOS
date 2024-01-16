// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/EventStore/EventStore.hpp>
#include <service-db/Settings.hpp>
#include <Units.hpp>

#include <cstdint>
#include <functional>
#include <string>

namespace app
{
    class ApplicationCommon;
}

namespace app
{
    namespace constants
    {
        inline constexpr units::SOC lowBatteryInfoThreshold{10};
    }

    class AbstractBatteryModel
    {
      public:
        virtual ~AbstractBatteryModel() noexcept = default;

        virtual Store::Battery getLevelState() const = 0;
        virtual bool isBatteryCharging(Store::Battery::State state) const = 0;
    };

    class BatteryModel : public AbstractBatteryModel
    {
      public:
        explicit BatteryModel(app::ApplicationCommon *app);
        Store::Battery getLevelState() const;
        bool isBatteryCharging(Store::Battery::State state) const;

      private:
        mutable settings::Settings settings;
    };
} // namespace app
