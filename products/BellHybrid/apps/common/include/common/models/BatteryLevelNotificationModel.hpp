// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <module-utils/EventStore/EventStore.hpp>
#include <service-db/Settings.hpp>

#include <cstdint>
#include <functional>
#include <string>
#include <Units.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app
{
    enum class BatteryLevelInterval
    {
        Unknown,
        Above15Percent, // 16-100%
        Below15Percent, // 10-15%
        Below10Percent, // 5-9%
        Below5Percent,  // 0-4%
    };

    class AbstractBatteryLevelNotificationModel
    {
      public:
        virtual ~AbstractBatteryLevelNotificationModel() noexcept = default;

        virtual void updateBatteryLevelInterval(units::SOC currentBatteryLevel)    = 0;
        [[nodiscard]] virtual BatteryLevelInterval getBatteryLevelInterval() const = 0;
        [[nodiscard]] virtual bool isBatteryIntervalHandled() const                = 0;
        virtual void setCurrentBatteryIntervalAsHandled()                          = 0;
    };

    class BatteryLevelNotificationModel : public AbstractBatteryLevelNotificationModel
    {
        static constexpr auto batteryLevelIntervalUpperHysteresis = 2;
        static constexpr auto above15PercentMinVal                = 16;
        static constexpr auto below15PercentMaxVal                = 15;
        static constexpr auto below10PercentMaxVal                = 9;
        static constexpr auto below5PercentMaxVal                 = 4;

      public:
        explicit BatteryLevelNotificationModel();
        void updateBatteryLevelInterval(units::SOC currentBatteryLevel) override;
        [[nodiscard]] BatteryLevelInterval getBatteryLevelInterval() const override;
        [[nodiscard]] bool isBatteryIntervalHandled() const override;
        void setCurrentBatteryIntervalAsHandled() override;

      private:
        BatteryLevelInterval currentBatteryLevelInterval = BatteryLevelInterval::Unknown;
        bool isCurrentBatteryIntervalHandled             = false;
    };
} // namespace app
