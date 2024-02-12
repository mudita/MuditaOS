// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LowBatteryInfoModel.hpp>
#include <memory>
#include <common/models/AbstractAlarmModel.hpp>

namespace gui
{
    class ListItemProvider;
}

namespace app
{
    class ApplicationCommon;
}

namespace settings
{
    class Settings;
}

namespace app::powernap
{
    class PowerNapModel;
    class PowerNapMainWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };

        class Presenter : public BasePresenter<PowerNapMainWindowContract::View>
        {
          public:
            virtual auto getNapTimeProvider() -> std::shared_ptr<gui::ListItemProvider> = 0;

            virtual void loadNapTimeList()                   = 0;
            virtual void activate()                          = 0;
            virtual bool handleIfPreWakeupIsToTurnOffFirst() = 0;
        };
    };

    class PowerNapMainWindowPresenter : public PowerNapMainWindowContract::Presenter
    {
      public:
        PowerNapMainWindowPresenter(app::ApplicationCommon *app,
                                    settings::Settings *settings,
                                    AbstractBatteryModel &batteryModel,
                                    AbstractLowBatteryInfoModel &lowBatteryInfoModel,
                                    AbstractAlarmModel &alarm);
        auto getNapTimeProvider() -> std::shared_ptr<gui::ListItemProvider> override;

        void loadNapTimeList() override;
        void activate() override;

      private:
        bool isBatteryCharging(Store::Battery::State state) const;
        bool isBatteryBelowLowLevelThreshold(units::SOC soc) const;
        void conditionalSwitchScreen();
        bool handleIfPreWakeupIsToTurnOffFirst() override;

        app::ApplicationCommon *app{nullptr};
        settings::Settings *settings{nullptr};
        std::shared_ptr<PowerNapModel> model;
        AbstractBatteryModel &batteryModel;
        AbstractLowBatteryInfoModel &lowBatteryInfoModel;
        AbstractAlarmModel &alarmModel;
    };
} // namespace app::powernap
