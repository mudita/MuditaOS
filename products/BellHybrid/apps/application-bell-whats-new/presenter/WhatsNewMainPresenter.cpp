// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "WhatsNewMainPresenter.hpp"
#include "WhatsNewCommon.hpp"
#include <apps-common/ApplicationCommon.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LowBatteryInfoModel.hpp>
#include <common/data/BatteryStatusSwitchData.hpp>
#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <product/version.hpp>

namespace app::whatsnew
{
    WhatsNewMainPresenter::WhatsNewMainPresenter(app::ApplicationCommon *app,
                                                 models::WhatsNewFeaturesModel &model,
                                                 AbstractBatteryModel &batteryModel,
                                                 AbstractLowBatteryInfoModel &lowBatteryInfoModel,
                                                 settings::Settings *settings)
        : app{app}, model{model}, batteryModel{batteryModel}, lowBatteryInfoModel{lowBatteryInfoModel}, settings{
                                                                                                            settings}
    {}

    auto WhatsNewMainPresenter::setCurrentOsVersion() -> void
    {
        settings->setValue(settings::SystemProperties::osCurrentVersion, VERSION, settings::SettingsScope::Global);
    }

    auto WhatsNewMainPresenter::getFeaturesCount() -> bool
    {
        return model.getFeatures().size();
    }

    auto WhatsNewMainPresenter::showFeatures() -> void
    {
        auto switchToNextScreen = [this]() { app->switchWindow(window::name::features); };

        const auto batteryState = batteryModel.getLevelState();
        const units::SOC soc    = batteryState.level;
        const bool isCharging   = batteryModel.isBatteryCharging(batteryState.state);
        if (not lowBatteryInfoModel.isInfoHandled() && not isCharging && soc < constants::lowBatteryInfoThreshold) {
            auto lowBatterySwitchData =
                std::make_unique<gui::AppsBatteryStatusSwitchData>(soc, isCharging, switchToNextScreen);
            app->switchWindow(window::name::lowBattery, std::move(lowBatterySwitchData));
            lowBatteryInfoModel.handleInfo();
        }
        else {
            switchToNextScreen();
        }
    }
} // namespace app::whatsnew
