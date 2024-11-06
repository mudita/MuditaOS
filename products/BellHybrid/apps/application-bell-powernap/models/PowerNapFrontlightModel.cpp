// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "PowerNapFrontlightModel.hpp"

#include <db/SystemSettings.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>
#include <FrontlightUtils.hpp>
#include <Utils.hpp>

namespace app::powernap
{
    PowerNapFrontlightModel::PowerNapFrontlightModel(ApplicationCommon *app, const std::chrono::seconds &alarmDuration)
        : app{app}, alarmDuration{alarmDuration}
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }

    auto PowerNapFrontlightModel::isAlarmLightEnabled() const -> bool
    {
        const auto alarmLightEnabled =
            settings.getValue(bell::settings::Alarm::lightActive, settings::SettingsScope::Global);
        return (utils::toNumeric(alarmLightEnabled) != 0);
    }

    auto PowerNapFrontlightModel::lockKeypressTrigger() -> void
    {
        app->bus.sendUnicast(
            std::make_shared<sevm::ScreenLightControlMessage>(
                screen_light_control::Action::ignoreKeypress, std::nullopt, screen_light_control::Sender::PowerNap),
            service::name::evt_manager);
    }

    auto PowerNapFrontlightModel::unlockKeypressTrigger() -> void
    {
        app->bus.sendUnicast(
            std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::stopIgnoringKeypress,
                                                              std::nullopt,
                                                              screen_light_control::Sender::PowerNap),
            service::name::evt_manager);
    }

    auto PowerNapFrontlightModel::startBrightnessFadeIn() -> void
    {
        const auto &fadeInParams = prepareFadeInParameters();
        app->bus.sendUnicast(std::make_shared<sevm::ScreenLightSetAutoProgressiveModeParams>(fadeInParams),
                             service::name::evt_manager);
        app->bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::turnOn,
                                                                               std::nullopt,
                                                                               screen_light_control::Sender::PowerNap),
                             service::name::evt_manager);
    }

    auto PowerNapFrontlightModel::turnOff() -> void
    {
        app->bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::turnOff,
                                                                               std::nullopt,
                                                                               screen_light_control::Sender::PowerNap),
                             service::name::evt_manager);
    }

    auto PowerNapFrontlightModel::getAlarmBrightness() const -> float
    {
        const auto &brightnessString =
            settings.getValue(bell::settings::Alarm::brightness, settings::SettingsScope::Global);
        return utils::frontlight::fixedValToPercentage(utils::toNumeric(brightnessString));
    }

    auto PowerNapFrontlightModel::prepareFadeInParameters() const -> screen_light_control::LinearProgressModeParameters
    {
        constexpr auto targetReachTimeOffset = std::chrono::minutes{1};
        constexpr auto startFunctionDuration = std::chrono::seconds{5};

#if defined(CONFIG_VERSION_PRO) && (CONFIG_VERSION_PRO == 1)
        constexpr auto startFunctionTarget = 5.0f;
#else
        constexpr auto startFunctionTarget = 10.0f;
#endif
        const auto brightness          = getAlarmBrightness();
        const auto endFunctionDuration = alarmDuration - targetReachTimeOffset - startFunctionDuration;

        const screen_light_control::functions::LinearProgressFunction startFunction{.target   = startFunctionTarget,
                                                                                    .duration = startFunctionDuration};
        const screen_light_control::functions::LinearProgressFunction endFunction{.target   = brightness,
                                                                                  .duration = endFunctionDuration};

        return screen_light_control::LinearProgressModeParameters{
            .startBrightnessValue = 0.0f, .functions = {startFunction, endFunction}, .brightnessHysteresis = 0.0f};
    }
} // namespace app::powernap
