// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrontlightAction.hpp"
#include "BellAlarmConstants.hpp"

#include <service-evtmgr/ScreenLightControlMessage.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>

#include <service-db/Settings.hpp>
#include <db/SystemSettings.hpp>
#include <module-utils/utility/Utils.hpp>

namespace alarms
{
    namespace
    {
        constexpr std::string_view alarmFrontlightOff     = "0";
        constexpr std::string_view prewakeupFrontlightOff = "0";

        /* Reach and maintain target brightness 1 minute faster than
         * the duration of the action. */
        constexpr auto frontlightRampDurationOffset = std::chrono::minutes{1};
        constexpr auto initialTargetDuration        = std::chrono::seconds{5};
        constexpr auto initialZeroBrightnessValue   = 10;

        void validateBrightness(std::string &brightness)
        {
            constexpr auto defaultBrightness = std::string_view{"50.0"};

            if (brightness.empty()) {
                brightness = defaultBrightness;
            }
        }

        screen_light_control::Sender translateDependency(FrontlightAction::SettingsDependency dependency)
        {
            screen_light_control::Sender sender;

            switch (dependency) {
            case FrontlightAction::SettingsDependency::AlarmClock:
                sender = screen_light_control::Sender::Alarm;
                break;
            case FrontlightAction::SettingsDependency::Prewakeup:
                sender = screen_light_control::Sender::AlarmPrewakeup;
                break;
            case FrontlightAction::SettingsDependency::None:
            default:
                sender = screen_light_control::Sender::Other;
                break;
            }
            return sender;
        }

        class ManualFrontlightAction : public AbstractAlarmAction
        {
          public:
            explicit ManualFrontlightAction(sys::Service &service, FrontlightAction::SettingsDependency dependency);
            bool execute() override;
            bool turnOff() override;

          private:
            screen_light_control::ManualModeParameters prepareParameters();

            sys::Service &service;
            settings::Settings settings;
            FrontlightAction::SettingsDependency settingsDependency;
        };

        class LinearProgressFrontlightAction : public AbstractAlarmAction
        {
          public:
            explicit LinearProgressFrontlightAction(sys::Service &service,
                                                    FrontlightAction::SettingsDependency dependency);
            bool execute() override;
            bool turnOff() override;

          private:
            screen_light_control::LinearProgressModeParameters prepareParameters();
            screen_light_control::LinearProgressModeParameters prepareAlarmClockParameters();
            screen_light_control::LinearProgressModeParameters preparePrewakeupParameters();

            sys::Service &service;
            settings::Settings settings;
            FrontlightAction::SettingsDependency settingsDependency;
        };

        std::unique_ptr<AbstractAlarmAction> createFrontlightImplementation(
            sys::Service &service, FrontlightAction::Mode mode, FrontlightAction::SettingsDependency settingsDependency)
        {
            switch (mode) {
            case FrontlightAction::Mode::Manual:
                return std::make_unique<ManualFrontlightAction>(service, settingsDependency);
            case FrontlightAction::Mode::LinearProgress:
                return std::make_unique<LinearProgressFrontlightAction>(service, settingsDependency);
            }
            return nullptr;
        }
    } // namespace

    FrontlightAction::FrontlightAction(sys::Service &service, Mode mode, SettingsDependency settingsDependency)
        : service{service}, settingsDependency{settingsDependency},
          pimpl{createFrontlightImplementation(service, mode, settingsDependency)}, settings{service::ServiceProxy{
                                                                                        service.weak_from_this()}}
    {}

    bool FrontlightAction::execute()
    {
        std::string settingString;

        switch (settingsDependency) {
        case SettingsDependency::AlarmClock:
            settingString = settings.getValue(bell::settings::Alarm::lightActive, settings::SettingsScope::Global);
            if (settingString == alarmFrontlightOff) {
                service.bus.sendUnicast(
                    std::make_shared<sevm::ScreenLightControlMessage>(screen_light_control::Action::ignoreKeypress),
                    service::name::evt_manager);
                return true;
            }
            break;
        case SettingsDependency::Prewakeup:
            settingString =
                settings.getValue(bell::settings::PrewakeUp::lightDuration, settings::SettingsScope::Global);
            if (settingString == prewakeupFrontlightOff) {
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
        std::string settingString;

        switch (settingsDependency) {
        case SettingsDependency::AlarmClock:
            settingString = settings.getValue(bell::settings::Alarm::lightActive, settings::SettingsScope::Global);
            if (settingString == alarmFrontlightOff) {
                service.bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                                            screen_light_control::Action::stopIgnoringKeypress),
                                        service::name::evt_manager);
            }
            break;
        default:
            break;
        }

        return pimpl->turnOff();
    }

    screen_light_control::ManualModeParameters ManualFrontlightAction::prepareParameters()
    {
        using bsp::eink_frontlight::BrightnessPercentage;

        auto brightnessString = settings.getValue(bell::settings::Alarm::brightness, settings::SettingsScope::Global);
        validateBrightness(brightnessString);
        screen_light_control::ManualModeParameters params{};
        params.manualModeBrightness = static_cast<BrightnessPercentage>(utils::toNumeric(brightnessString));

        return params;
    }

    ManualFrontlightAction::ManualFrontlightAction(sys::Service &service,
                                                   FrontlightAction::SettingsDependency dependency)
        : service{service}, settingsDependency{dependency}
    {
        settings.init(service::ServiceProxy{service.weak_from_this()});
    }

    bool ManualFrontlightAction::execute()
    {
        const auto params = prepareParameters();
        const auto sender = translateDependency(settingsDependency);
        service.bus.sendUnicast(
            std::make_shared<sevm::ScreenLightControlMessage>(
                screen_light_control::Action::turnOn, screen_light_control::Parameters{params}, sender),
            service::name::evt_manager);
        return true;
    }

    bool ManualFrontlightAction::turnOff()
    {
        const auto sender = translateDependency(settingsDependency);
        service.bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                                    screen_light_control::Action::turnOff, std::nullopt, sender),
                                service::name::evt_manager);
        return true;
    }

    LinearProgressFrontlightAction::LinearProgressFrontlightAction(sys::Service &service,
                                                                   FrontlightAction::SettingsDependency dependency)
        : service{service}, settingsDependency{dependency}
    {
        settings.init(service::ServiceProxy{service.weak_from_this()});
    }

    bool LinearProgressFrontlightAction::execute()
    {
        const auto params = prepareParameters();
        const auto sender = translateDependency(settingsDependency);
        service.bus.sendUnicast(std::make_shared<sevm::ScreenLightSetAutoProgressiveModeParams>(params),
                                service::name::evt_manager);
        service.bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                                    screen_light_control::Action::turnOn, std::nullopt, sender),
                                service::name::evt_manager);
        return true;
    }

    screen_light_control::LinearProgressModeParameters LinearProgressFrontlightAction::prepareParameters()
    {
        switch (settingsDependency) {
        case FrontlightAction::SettingsDependency::AlarmClock:
            return prepareAlarmClockParameters();
        case FrontlightAction::SettingsDependency::Prewakeup:
            return preparePrewakeupParameters();
        default:
            return {};
        }
    }

    screen_light_control::LinearProgressModeParameters LinearProgressFrontlightAction::prepareAlarmClockParameters()
    {
        constexpr auto mainTargetDuration =
            defaultAlarmRingingTime - frontlightRampDurationOffset - initialTargetDuration;

        auto initialBrightness        = initialZeroBrightnessValue;
        const auto isPrewakeUpEnabled = (settings.getValue(bell::settings::PrewakeUp::lightDuration,
                                                           settings::SettingsScope::Global) != prewakeupFrontlightOff);
        if (isPrewakeUpEnabled) {
            auto initialBrightnessString =
                settings.getValue(bell::settings::PrewakeUp::brightness, settings::SettingsScope::Global);
            validateBrightness(initialBrightnessString);
            initialBrightness = utils::toNumeric(initialBrightnessString);
        }

        auto targetBrightnessString =
            settings.getValue(bell::settings::Alarm::brightness, settings::SettingsScope::Global);
        validateBrightness(targetBrightnessString);

        const screen_light_control::functions::LinearProgressFunction startFunction{
            .target = static_cast<float>(initialBrightness), .duration = initialTargetDuration};
        const screen_light_control::functions::LinearProgressFunction endFunction{
            .target = static_cast<float>(utils::toNumeric(targetBrightnessString)), .duration = mainTargetDuration};

        return screen_light_control::LinearProgressModeParameters{.startBrightnessValue =
                                                                      static_cast<float>(initialBrightness),
                                                                  .functions            = {startFunction, endFunction},
                                                                  .brightnessHysteresis = 0.0f};
    }

    screen_light_control::LinearProgressModeParameters LinearProgressFrontlightAction::preparePrewakeupParameters()
    {
        auto brightnessString =
            settings.getValue(bell::settings::PrewakeUp::brightness, settings::SettingsScope::Global);
        validateBrightness(brightnessString);
        const auto value = settings.getValue(bell::settings::PrewakeUp::lightDuration, settings::SettingsScope::Global);
        const auto lightDuration      = std::chrono::minutes{utils::toNumeric(value)};
        const auto mainTargetDuration = lightDuration - frontlightRampDurationOffset - initialTargetDuration;

        const screen_light_control::functions::LinearProgressFunction startFunction{
            .target = initialZeroBrightnessValue, .duration = initialTargetDuration};
        const screen_light_control::functions::LinearProgressFunction endFunction{
            .target = static_cast<float>(utils::toNumeric(brightnessString)), .duration = mainTargetDuration};

        return screen_light_control::LinearProgressModeParameters{
            .startBrightnessValue = 0.0f, .functions = {startFunction, endFunction}, .brightnessHysteresis = 0.0f};
    }

    bool LinearProgressFrontlightAction::turnOff()
    {
        const auto sender = translateDependency(settingsDependency);
        service.bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                                    screen_light_control::Action::turnOff, std::nullopt, sender),
                                service::name::evt_manager);
        return true;
    }
} // namespace alarms
