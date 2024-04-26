// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MainFrontlightModel.hpp"

#include <db/SystemSettings.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>

namespace app
{
    MainFrontlightModel::MainFrontlightModel(ApplicationCommon *app) : app{app}
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }

    void MainFrontlightModel::LockKeypressTrigger()
    {
        app->bus.sendUnicast(
            std::make_shared<sevm::ScreenLightControlMessage>(
                screen_light_control::Action::ignoreKeypress, std::nullopt, screen_light_control::Sender::Other),
            service::name::evt_manager);
    }

    void MainFrontlightModel::UnlockKeypressTrigger()
    {
        app->bus.sendUnicast(
            std::make_shared<sevm::ScreenLightControlMessage>(
                screen_light_control::Action::stopIgnoringKeypress, std::nullopt, screen_light_control::Sender::Other),
            service::name::evt_manager);
    }

    void MainFrontlightModel::StartBrightnessFadeIn(std::chrono::seconds duration)
    {
        const auto &fadeInParams = PrepareFadeInParameters(duration);
        app->bus.sendUnicast(std::make_shared<sevm::ScreenLightSetAutoProgressiveModeParams>(fadeInParams),
                             service::name::evt_manager);
        app->bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::turnOn,
                                                                               std::nullopt,
                                                                               screen_light_control::Sender::Other),
                             service::name::evt_manager);
    }

    void MainFrontlightModel::TurnOff()
    {
        app->bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::turnOff,
                                                                               std::nullopt,
                                                                               screen_light_control::Sender::Other),
                             service::name::evt_manager);
    }

    int MainFrontlightModel::GetAlarmBrightness() const
    {
        const auto brightness = settings.getValue(bell::settings::Alarm::brightness, settings::SettingsScope::Global);
        return utils::toNumeric(brightness);
    }

    MainFrontlightModel::FadeInParams MainFrontlightModel::PrepareFadeInParameters(std::chrono::seconds duration) const
    {
        using FadeInFunction                 = screen_light_control::functions::LinearProgressFunction;
        constexpr auto startFunctionDuration = std::chrono::seconds{5};
        constexpr auto startFunctionTarget   = 10.0f;
        const auto brightness                = GetAlarmBrightness();

        const FadeInFunction startFunction{.target = startFunctionTarget, .duration = startFunctionDuration};
        const FadeInFunction endFunction{.target = static_cast<float>(brightness), .duration = duration};

        return FadeInParams{
            .startBrightnessValue = 0.0f, .functions = {startFunction, endFunction}, .brightnessHysteresis = 0.0f};
    }
} // namespace app
