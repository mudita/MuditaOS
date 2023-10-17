// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMeditation.hpp"
#include "MeditationOptionsWindows.hpp"
#include "MeditationTimerWindow.hpp"
#include "MeditationWindow.hpp"
#include "Names.hpp"
#include "MeditationParams.hpp"
#include "Constants.hpp"

#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

namespace app
{
    ApplicationMeditation::ApplicationMeditation(std::string name,
                                                 std::string parent,
                                                 StatusIndicators statusIndicators,
                                                 StartInBackground startInBackground)
        : Application{std::move(name), std::move(parent), statusIndicators, startInBackground}
    {}

    auto ApplicationMeditation::InitHandler() -> sys::ReturnCodes
    {
        const auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        const auto counterVisible =
            getLocalSettingsValue(settings::Meditation::showCounter, Constants::Params::defaultCounterVisible);
        auto counterVisibleOnChangeCallback = [this](bool newValue) {
            settings->setValue(
                settings::Meditation::showCounter, utils::to_string(newValue), settings::SettingsScope::AppLocal);
        };

        const auto preparationTime           = std::chrono::seconds(getLocalSettingsValue(
            settings::Meditation::preparationTime, Constants::Params::defaultPreparationTime.count()));
        auto preparationTimeOnChangeCallback = [this](std::chrono::seconds newValue) {
            settings->setValue(settings::Meditation::preparationTime,
                               utils::to_string(newValue.count()),
                               settings::SettingsScope::AppLocal);
        };
        gui::OptionsData::OptionParams params = {
            .preparationTime{preparationTime, std::move(preparationTimeOnChangeCallback)},
            .showCounter{counterVisible, std::move(counterVisibleOnChangeCallback)}};

        state = std::make_unique<gui::OptionsData>(std::move(params));

        createUserInterface();

        return ret;
    }

    auto ApplicationMeditation::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
        -> sys::MessagePointer
    {
        const auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (retMsg && (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }

    auto ApplicationMeditation::SwitchPowerModeHandler([[maybe_unused]] sys::ServicePowerMode mode) -> sys::ReturnCodes
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationMeditation::createUserInterface()
    {
        windowsFactory.attach(
            app::window::name::meditation_main_window, [&](ApplicationCommon *app, const std::string &name) {
                const auto durationInitValue =
                    getLocalSettingsValue(settings::Meditation::duration, Constants::Params::defaultMeditationDuration);
                auto durationOnChangeCallback = [this](std::int32_t newValue) {
                    settings->setValue(
                        settings::Meditation::duration, utils::to_string(newValue), settings::SettingsScope::AppLocal);
                };
                const auto intervalChimeInitValue  = std::chrono::minutes(getLocalSettingsValue(
                    settings::Meditation::intervalChime, Constants::Params::defaultChimeInterval.count()));
                auto intervalChimeOnChangeCallback = [this](std::chrono::minutes newValue) {
                    settings->setValue(settings::Meditation::intervalChime,
                                       utils::to_string(newValue.count()),
                                       settings::SettingsScope::AppLocal);
                };
                MeditationParams params = {
                    .meditationDuration{.initValue        = durationInitValue,
                                        .onChangeCallback = std::move(durationOnChangeCallback)},
                    .intervalChime{.initValue        = intervalChimeInitValue,
                                   .onChangeCallback = std::move(intervalChimeOnChangeCallback)}};

                return std::make_unique<gui::MeditationWindow>(app, std::move(params));
            });
        windowsFactory.attach(app::window::name::meditation_timer, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::MeditationTimerWindow>(app);
        });
        windowsFactory.attach(app::window::name::meditation_options,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::MeditationOptionsWindow>(app);
                              });
        windowsFactory.attach(app::window::name::meditation_preparation,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::PreparationTimeWindow>(app);
                              });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::BluetoothAuthenticate,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::PhoneLock,
                      gui::popup::ID::SimLock,
                      gui::popup::ID::Alarm});
    }

    void ApplicationMeditation::destroyUserInterface()
    {}

    template <typename T>
    T ApplicationMeditation::getLocalSettingsValue(const std::string &variableName, T defaultValue)
    {
        const auto &value = settings->getValue(variableName, settings::SettingsScope::AppLocal);
        return value.empty() ? defaultValue : utils::getNumericValue<T>(value);
    }
} // namespace app
