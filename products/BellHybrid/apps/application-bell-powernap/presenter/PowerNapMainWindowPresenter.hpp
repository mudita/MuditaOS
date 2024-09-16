// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LowBatteryInfoModel.hpp>
#include <memory>

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

            virtual void loadNapTimeList() = 0;
            virtual void activate()        = 0;
        };
    };

    class PowerNapMainWindowPresenter : public PowerNapMainWindowContract::Presenter
    {
      public:
        PowerNapMainWindowPresenter(app::ApplicationCommon *app,
                                    settings::Settings *settings,
                                    AbstractBatteryModel &batteryModel,
                                    AbstractLowBatteryInfoModel &lowBatteryInfoModel);
        auto getNapTimeProvider() -> std::shared_ptr<gui::ListItemProvider> override;

        void loadNapTimeList() override;
        void activate() override;

      private:
        bool isBatteryCharging(Store::Battery::State state) const;
        bool isBatteryBelowLowLevelThreshold(units::SOC soc) const;
        void conditionalSwitchScreen();

        app::ApplicationCommon *app{nullptr};
        settings::Settings *settings{nullptr};
        std::shared_ptr<PowerNapModel> model;
        AbstractBatteryModel &batteryModel;
        AbstractLowBatteryInfoModel &lowBatteryInfoModel;
    };
} // namespace app::powernap
