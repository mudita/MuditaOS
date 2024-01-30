// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapMainWindowPresenter.hpp"
#include "models/PowerNapModel.hpp"
#include "data/PowerNapCommon.hpp"
#include <ApplicationBellPowerNap.hpp>

namespace app::powernap
{
    PowerNapMainWindowPresenter::PowerNapMainWindowPresenter(app::ApplicationCommon *app,
                                                             settings::Settings *settings,
                                                             AbstractBatteryModel &batteryModel,
                                                             AbstractLowBatteryInfoModel &lowBatteryInfoModel)
        : app{app}, settings{settings}, model{std::make_shared<PowerNapModel>()}, batteryModel{batteryModel},
          lowBatteryInfoModel{lowBatteryInfoModel}
    {}

    auto PowerNapMainWindowPresenter::getNapTimeProvider() -> std::shared_ptr<gui::ListItemProvider>
    {
        return model;
    }

    void PowerNapMainWindowPresenter::loadNapTimeList()
    {
        const auto value = settings->getValue(powernapDBRecordName, settings::SettingsScope::AppLocal);
        model->createData();
        model->setValue(std::chrono::minutes{utils::getNumericValue<int>(value)});
    }

    void PowerNapMainWindowPresenter::activate()
    {
        const auto currentValue = model->getCurrentValue();
        settings->setValue(
            powernapDBRecordName, utils::to_string(currentValue.count()), settings::SettingsScope::AppLocal);
        static_cast<app::Application *>(app)->suspendIdleTimer();
        conditionalSwitchScreen();
    }

    void PowerNapMainWindowPresenter::conditionalSwitchScreen()
    {
        const auto batteryState = batteryModel.getLevelState();
        if (not lowBatteryInfoModel.isInfoHandled() && not isBatteryCharging(batteryState.state) &&
            isBatteryBelowLowLevelThreshold(batteryState.level)) {
            app->switchWindow(gui::window::name::powerNapWarning);
            lowBatteryInfoModel.handleInfo();
        }
        else {
            app->switchWindow(gui::window::name::powerNapProgress);
        }
    }

    bool PowerNapMainWindowPresenter::isBatteryCharging(Store::Battery::State state) const
    {
        return batteryModel.isBatteryCharging(state);
    }

    bool PowerNapMainWindowPresenter::isBatteryBelowLowLevelThreshold(units::SOC soc) const
    {
        return soc < constants::lowBatteryInfoThreshold;
    }
} // namespace app::powernap
