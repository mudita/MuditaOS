// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrontlightAction.hpp"

#include <service-evtmgr/ScreenLightControlMessage.hpp>
#include <service-evtmgr/Constants.hpp>

#include <service-db/Settings.hpp>
#include <db/SystemSettings.hpp>
#include <module-utils/utility/Utils.hpp>

namespace alarms
{
    namespace
    {
        class ManualFrontlightAction : public AbstractAlarmAction
        {
          public:
            explicit ManualFrontlightAction(sys::Service &service);
            bool execute() override;
            bool turnOff() override;

          private:
            screen_light_control::ManualModeParameters prepareParameters();

            sys::Service &service;
        };

        class LinearProgressFrontlightAction : public AbstractAlarmAction
        {
          public:
            explicit LinearProgressFrontlightAction(sys::Service &service);
            bool execute() override;
            bool turnOff() override;

          private:
            screen_light_control::LinearProgressModeParameters prepareParameters();

            sys::Service &service;
            settings::Settings settings;
        };

        std::unique_ptr<AbstractAlarmAction> createFrontlightImplementation(sys::Service &service,
                                                                            FrontlightAction::Mode mode)
        {
            switch (mode) {
            case FrontlightAction::Mode::Manual:
                return std::make_unique<ManualFrontlightAction>(service);
            case FrontlightAction::Mode::LinearProgress:
                return std::make_unique<LinearProgressFrontlightAction>(service);
            }
            return nullptr;
        }
    } // namespace

    FrontlightAction::FrontlightAction(sys::Service &service, Mode mode, SettingsDependency settingsDependency)
        : settingsDependency{settingsDependency}, pimpl{createFrontlightImplementation(service, mode)},
          settings{service::ServiceProxy{service.weak_from_this()}}
    {}

    bool FrontlightAction::execute()
    {
        std::string settingString;

        switch (settingsDependency) {
        case SettingsDependency::AlarmClock:
            settingString = settings.getValue(bell::settings::Alarm::lightActive, settings::SettingsScope::Global);
            if (settingString == std::string(alarmFrontlightOFF)) {
                return true;
            }
            break;
        case SettingsDependency::Prewakeup:
            settingString =
                settings.getValue(bell::settings::PrewakeUp::lightDuration, settings::SettingsScope::Global);
            if (settingString == std::string(prewakeupFrontlightOFF)) {
                return true;
            }
            break;
        case SettingsDependency::None:
            break;
        }
        return pimpl->execute();
    }

    bool FrontlightAction::turnOff()
    {
        return pimpl->turnOff();
    }

    ManualFrontlightAction::ManualFrontlightAction(sys::Service &service) : service{service}
    {}

    bool ManualFrontlightAction::execute()
    {
        auto params = prepareParameters();
        service.bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                                    screen_light_control::Action::turnOn, screen_light_control::Parameters{params}),
                                service::name::evt_manager);
        return true;
    }

    screen_light_control::ManualModeParameters ManualFrontlightAction::prepareParameters()
    {
        return screen_light_control::ManualModeParameters{.manualModeBrightness = 100.0f};
    }

    bool ManualFrontlightAction::turnOff()
    {
        service.bus.sendUnicast(
            std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::turnOff),
            service::name::evt_manager);
        return true;
    }

    LinearProgressFrontlightAction::LinearProgressFrontlightAction(sys::Service &service) : service{service}
    {
        settings.init(service::ServiceProxy{service.weak_from_this()});
    }

    bool LinearProgressFrontlightAction::execute()
    {
        const auto params = prepareParameters();
        service.bus.sendUnicast(std::make_shared<sevm::ScreenLightSetAutoProgressiveModeParams>(params),
                                service::name::evt_manager);
        service.bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::turnOn),
                                service::name::evt_manager);
        return true;
    }

    screen_light_control::LinearProgressModeParameters LinearProgressFrontlightAction::prepareParameters()
    {
        constexpr auto firstTargetDuration = std::chrono::seconds{5};
        const auto value = settings.getValue(bell::settings::PrewakeUp::lightDuration, settings::SettingsScope::Global);
        const auto lightDuration        = std::chrono::minutes{utils::toNumeric(value)};
        const auto secondTargetDuration = lightDuration - std::chrono::minutes{1} - firstTargetDuration;
        return screen_light_control::LinearProgressModeParameters{
            .startBrightnessValue = 0.0f,
            .functions            = {screen_light_control::functions::LinearProgressFunction{.target   = 10.0f,
                                                                                  .duration = firstTargetDuration},
                          screen_light_control::functions::LinearProgressFunction{.target   = 100.0f,
                                                                                  .duration = secondTargetDuration}},
            .brightnessHysteresis = 0.0f};
    }

    bool LinearProgressFrontlightAction::turnOff()
    {
        service.bus.sendUnicast(
            std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::turnOff),
            service::name::evt_manager);
        return true;
    }
} // namespace alarms
