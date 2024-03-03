// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <EventStore.hpp>
#include <Units.hpp>
#include <chrono>
#include <vector>

namespace gui
{
    class ListItemProvider;
}

namespace app
{
    class ApplicationCommon;
    class AbstractBatteryModel;
    class AbstractLowBatteryInfoModel;
}

namespace settings
{
    class Settings;
}

namespace app::relaxation
{
    class RelaxationTimerSelectContract
    {
      public:
        using Range = std::vector<std::chrono::minutes>;
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<RelaxationTimerSelectContract::View>
        {
          public:
            virtual ~Presenter()                                      = default;
            virtual const Range &getTimerValuesRange() const noexcept = 0;
            virtual std::chrono::minutes getCurrentTimerValue() const = 0;
            virtual void setTimerValue(std::chrono::minutes)          = 0;
            virtual Store::Battery getBatteryState()                     = 0;
            virtual bool isBatteryCharging(Store::Battery::State state) const  = 0;
            virtual bool isBatteryBelowLowLevelThreshold(units::SOC soc) const = 0;
            [[nodiscard]] virtual bool isLowBatteryWindowHandled() const = 0;
            virtual void handleLowBatteryWindow()                        = 0;
        };
    };

    class RelaxationTimerSelectPresenter : public RelaxationTimerSelectContract::Presenter
    {
        settings::Settings *settings{nullptr};
        AbstractBatteryModel &batteryModel;
        AbstractLowBatteryInfoModel &lowBatteryInfoModel;

        const RelaxationTimerSelectContract::Range &getTimerValuesRange() const noexcept override;
        std::chrono::minutes getCurrentTimerValue() const override;
        void setTimerValue(std::chrono::minutes) override;
        Store::Battery getBatteryState() override;
        bool isBatteryCharging(Store::Battery::State state) const override;
        bool isBatteryBelowLowLevelThreshold(units::SOC soc) const override;
        [[nodiscard]] bool isLowBatteryWindowHandled() const override;
        void handleLowBatteryWindow() override;

      public:
        RelaxationTimerSelectPresenter(settings::Settings *settings,
                                       AbstractBatteryModel &batteryModel,
                                       AbstractLowBatteryInfoModel &lowBatteryInfoModel);
    };
} // namespace app::relaxation
