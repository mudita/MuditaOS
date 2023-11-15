// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellOnBoarding.hpp"

#include <presenter/OnBoardingLanguageWindowPresenter.hpp>
#include <presenter/OnBoardingShortcutsWindowPresenter.hpp>
#include <windows/OnBoardingLanguageWindow.hpp>
#include <windows/OnBoardingFinalizeWindow.hpp>
#include <windows/OnBoardingOnOffWindow.hpp>
#include <windows/OnBoardingSettingsWindow.hpp>
#include <windows/OnBoardingWelcomeWindow.hpp>
#include <windows/OnBoardingInstructionPromptWindow.hpp>
#include <windows/OnBoardingShortcutsOptionWindow.hpp>

#include <service-appmgr/ServiceApplicationManagerName.hpp>
#include <service-appmgr/messages/GetCurrentDisplayLanguageResponse.hpp>

#include <apps-common/messages/OnBoardingMessages.hpp>
#include <common/BellPowerOffPresenter.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <common/windows/BellWelcomeWindow.hpp>
#include <common/windows/ShortcutsWindow.hpp>
#include <common/models/LayoutModel.hpp>

#include <application-bell-settings/models/TemperatureUnitModel.hpp>
#include <application-bell-settings/models/DateTimeUnitsModel.hpp>
#include <application-bell-settings/presenter/TimeUnitsPresenter.hpp>
#include <Timers/TimerFactory.hpp>
#include <AppMessage.hpp>

namespace
{
    constexpr auto informationPromptTimeout = std::chrono::seconds{15};
    constexpr auto informationTimerName     = "OnBoardingInformationTimer";
    constexpr auto userIdleTimeout          = std::chrono::minutes{3};
    constexpr auto userIdleTimerName        = "OnBoardingUserIdleTimer";

} // namespace

namespace app
{
    ApplicationBellOnBoarding::ApplicationBellOnBoarding(std::string name,
                                                         std::string parent,
                                                         StatusIndicators statusIndicators,
                                                         StartInBackground startInBackground,
                                                         std::uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth)
    {}

    ApplicationBellOnBoarding::~ApplicationBellOnBoarding()
    {
        if (informationPromptTimer.isValid()) {
            informationPromptTimer.stop();
            informationPromptTimer.reset();
        }
        if (userIdleTimer.isValid()) {
            userIdleTimer.stop();
            userIdleTimer.reset();
        }
    }

    sys::ReturnCodes ApplicationBellOnBoarding::InitHandler()
    {
        const auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();

        connect(typeid(manager::GetCurrentDisplayLanguageResponse), [&](sys::Message *msg) {
            if (gui::window::name::onBoardingLanguageWindow == getCurrentWindow()->getName()) {
                switchWindow(gui::window::name::onBoardingShortcutsOptionWindow);
                return sys::msgHandled();
            }
            return sys::msgNotHandled();
        });

        informationPromptTimer = sys::TimerFactory::createSingleShotTimer(
            this, informationTimerName, informationPromptTimeout, [this]([[maybe_unused]] sys::Timer &timer) {
                displayInformation(getCurrentWindow()->getName());
            });

        userIdleTimer = sys::TimerFactory::createSingleShotTimer(
            this, userIdleTimerName, userIdleTimeout, [this]([[maybe_unused]] sys::Timer &timer) {
                if (getCurrentWindow()->getName() == gui::window::name::onBoardingLanguageWindow) {
                    switchWindow(gui::name::window::main_window);
                }
            });

        userIdleTimer.start();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellOnBoarding::createUserInterface()
    {

        windowsFactory.attach(gui::name::window::main_window, [this](ApplicationCommon *app, const std::string &name) {
            auto powerOffPresenter = std::make_unique<gui::BellPowerOffPresenter>(app);
            return std::make_unique<gui::OnBoardingOnOffWindow>(app, std::move(powerOffPresenter), name);
        });

        windowsFactory.attach(
            gui::window::name::onBoardingLanguageWindow, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<OnBoarding::OnBoardingLanguageWindowPresenter>(this);
                return std::make_unique<gui::OnBoardingLanguageWindow>(app, std::move(presenter), name);
            });

        windowsFactory.attach(gui::window::name::onBoardingShortcutsOptionWindow,
                              [this](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::OnBoardingShortcutsOptionWindow>(app, name);
                              });

        windowsFactory.attach(
            gui::window::name::onBoardingShortcutsWindow, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<OnBoarding::OnBoardingShortcutsWindowPresenter>(this);
                return std::make_unique<gui::ShortcutsWindow>(app, std::move(presenter), name);
            });

        windowsFactory.attach(
            gui::window::name::onBoardingSettingsWindow, [this](ApplicationCommon *app, const std::string &name) {
                auto layoutModel          = std::make_unique<bell_settings::LayoutModel>(this);
                auto temperatureUnitModel = std::make_unique<bell_settings::TemperatureUnitModel>(app);
                auto timeUnitsProvider    = std::make_shared<bell_settings::DateTimeUnitsModelFactoryResetValues>(app);
                auto presenter            = std::make_unique<bell_settings::TimeUnitsWindowPresenter>(
                    this, timeUnitsProvider, std::move(temperatureUnitModel), std::move(layoutModel));
                return std::make_unique<gui::OnBoardingSettingsWindow>(app, std::move(presenter), name);
            });

        windowsFactory.attach(
            gui::window::name::finalizeOnBoardingWindow, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter =
                    std::make_unique<OnBoarding::OnBoardingFinalizeWindowPresenter>([&]() { finalizeOnBoarding(); });
                return std::make_unique<gui::OnBoardingFinalizeWindow>(app, std::move(presenter), name);
            });

        windowsFactory.attach(gui::window::name::informationOnBoardingWindow,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::OnBoardingInstructionPromptWindow>(app, name);
                              });

        attachPopups({gui::popup::ID::ChargingNotification, gui::popup::ID::ChargingDoneNotification});
    }

    void ApplicationBellOnBoarding::destroyUserInterface()
    {}

    sys::ReturnCodes ApplicationBellOnBoarding::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        return sys::ReturnCodes::Success;
    }

    sys::MessagePointer ApplicationBellOnBoarding::DataReceivedHandler(sys::DataMessage *msgl,
                                                                       sys::ResponseMessage *resp)
    {
        auto retMsg = sys::msgNotHandled();
        // DataReceiveHandler is deprecated so when it will be moved to connect this parts also have to be moved
        if (msgl->messageType == MessageType::AppInputEvent) {
            if (!handleInformationOnInputEvent(msgl)) {
                retMsg = ApplicationCommon::DataReceivedHandler(msgl);
            }
        }
        else if (msgl->messageType == MessageType::AppSwitchWindow) {
            handleInformationBeforeSwitchWindow(msgl);
            retMsg = ApplicationCommon::DataReceivedHandler(msgl);
            handleInformationAfterSwitchWindow();
        }
        else {
            retMsg = Application::DataReceivedHandler(msgl);
        }

        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }

    void ApplicationBellOnBoarding::finalizeOnBoarding()
    {
        bus.sendUnicast(std::make_shared<onBoarding::FinalizeOnBoarding>(), service::name::appmgr);
    }

    void ApplicationBellOnBoarding::displayInformation(const std::string &windowToReturn)
    {
        // If user is during language selection, pick new language for hint popup
        if (windowToReturn == gui::window::name::onBoardingLanguageWindow) {
            const auto languageSelectWindow = dynamic_cast<gui::OnBoardingLanguageWindow *>(getWindow(windowToReturn));
            const auto selectedLang         = languageSelectWindow->getSelectedLanguage();

            if (utils::getDisplayLanguage() != selectedLang) {
                utils::setDisplayLanguage(selectedLang);
            }
        }

        const auto [icon, text] = getDisplayDataFromState();
        switchWindow(
            gui::window::name::informationOnBoardingWindow,
            gui::BellFinishedWindowData::Factory::create(icon,
                                                         windowToReturn,
                                                         utils::translate(text),
                                                         gui::BellFinishedWindowData::ExitBehaviour::SwitchWindow,
                                                         std::chrono::seconds::zero()));
    }

    OnBoarding::InformationDisplay ApplicationBellOnBoarding::getDisplayDataFromState()
    {
        switch (informationState) {
        case OnBoarding::InformationStates::RotateInfo:
            return {"button_rotate_W_G", "app_bell_onboarding_info_rotate"};
        case OnBoarding::InformationStates::LightClickInfo:
            return {"button_light_press_W_G", "app_bell_onboarding_info_light_click"};
        case OnBoarding::InformationStates::DeepClickWarningInfo:
            return {"button_deep_press_W_G", "app_bell_onboarding_info_deep_click_warning"};
        case OnBoarding::InformationStates::DeepClickCorrectionInfo:
            return {"button_light_press_W_G", "app_bell_onboarding_info_deep_click_correction"};
        }
        return {"", ""};
    }

    bool ApplicationBellOnBoarding::isInformationPromptPermittedOnCurrentWindow()
    {
        const auto currentWindow = getCurrentWindow()->getName();
        return (currentWindow != gui::name::window::main_window &&
                currentWindow != gui::window::name::finalizeOnBoardingWindow &&
                (currentWindow != gui::window::name::informationOnBoardingWindow ||
                 informationState == OnBoarding::InformationStates::DeepClickWarningInfo ||
                 informationState == OnBoarding::InformationStates::DeepClickCorrectionInfo));
    }

    void ApplicationBellOnBoarding::startTimerOnWindows()
    {
        if (isInformationPromptPermittedOnCurrentWindow()) {
            informationPromptTimer.start();
        }
    }

    void ApplicationBellOnBoarding::restartTimerOnWindows()
    {
        if (isInformationPromptPermittedOnCurrentWindow()) {
            informationPromptTimer.stop();
            informationPromptTimer.start();
        }
    }

    void ApplicationBellOnBoarding::handleInformationBeforeSwitchWindow(sys::DataMessage *msgl)
    {
        const auto msg = static_cast<AppSwitchWindowMessage *>(msgl);

        informationPromptTimer.stop();

        auto selectedWindowCondition =
            getCurrentWindow()->getName() == gui::window::name::informationOnBoardingWindow &&
            msg->getWindowName() == *getPreviousWindow() &&
            msg->getSenderWindowName() == gui::window::name::informationOnBoardingWindow;

        if (selectedWindowCondition && informationState == OnBoarding::InformationStates::DeepClickWarningInfo) {

            informationState = OnBoarding::InformationStates::DeepClickCorrectionInfo;
            displayInformation(msg->getWindowName());
        }
        else if (selectedWindowCondition &&
                 informationState == OnBoarding::InformationStates::DeepClickCorrectionInfo) {
            informationState = OnBoarding::InformationStates::RotateInfo;
        }
    }

    void ApplicationBellOnBoarding::handleInformationAfterSwitchWindow()
    {
        startTimerOnWindows();
    }

    bool ApplicationBellOnBoarding::handleInformationOnInputEvent(sys::DataMessage *msgl)
    {
        const auto inputEvent = static_cast<AppInputEventMessage *>(msgl)->getEvent();
        restartTimerOnWindows();
        userIdleTimer.restart(userIdleTimeout);

        if (isInformationPromptPermittedOnCurrentWindow()) {
            if (inputEvent.isKeyRelease(gui::KeyCode::KEY_UP) || inputEvent.isKeyRelease(gui::KeyCode::KEY_DOWN)) {
                gui::AppWindow *window = getCurrentWindow();
                const auto shortcutsWindow = window->getName() == gui::window::name::onBoardingShortcutsWindow
                                                 ? dynamic_cast<gui::ShortcutsWindow *>(window)
                                                 : nullptr;
                if (shortcutsWindow != nullptr) {
                    if (shortcutsWindow->isOneOfTwoLastShortcuts() && inputEvent.isKeyRelease(gui::KeyCode::KEY_UP))
                        informationState = OnBoarding::InformationStates::LightClickInfo;
                    else
                        informationState = OnBoarding::InformationStates::RotateInfo;
                }
                else {
                    informationState = OnBoarding::InformationStates::LightClickInfo;
                }
            }
            else if (getCurrentWindow()->getName() != gui::window::name::onBoardingShortcutsWindow) {
                if (inputEvent.isKeyRelease(gui::KeyCode::KEY_RIGHT) ||
                    inputEvent.isKeyRelease(gui::KeyCode::KEY_LEFT)) {
                    informationState = OnBoarding::InformationStates::DeepClickWarningInfo;
                    if (getCurrentWindow()->getName() == gui::window::name::informationOnBoardingWindow) {
                        displayInformation(*getPreviousWindow());
                    }
                    else {
                        displayInformation(getCurrentWindow()->getName());
                    }
                }
                else if (inputEvent.isKeyRelease(gui::KeyCode::KEY_ENTER)) {
                    if (informationState == OnBoarding::InformationStates::DeepClickWarningInfo) {
                        informationPromptTimer.stop();
                        informationState = OnBoarding::InformationStates::DeepClickCorrectionInfo;
                        displayInformation(*getPreviousWindow());
                    }
                    else {
                        informationState = OnBoarding::InformationStates::RotateInfo;
                    }
                }
            }
        }
        else {
            if (getCurrentWindow()->getName() == gui::window::name::informationOnBoardingWindow) {
                switch (informationState) {
                case OnBoarding::InformationStates::DeepClickCorrectionInfo:
                    if (inputEvent.isKeyRelease(gui::KeyCode::KEY_ENTER)) {
                        switchWindow(*getPreviousWindow());
                        return true;
                    }
                    break;
                case OnBoarding::InformationStates::LightClickInfo:
                    if (inputEvent.isKeyRelease(gui::KeyCode::KEY_ENTER)) {
                        switchWindow(*getPreviousWindow());
                        return true;
                    }
                    break;
                case OnBoarding::InformationStates::RotateInfo:
                    if (inputEvent.isKeyRelease(gui::KeyCode::KEY_UP) ||
                        inputEvent.isKeyRelease(gui::KeyCode::KEY_DOWN) ||
                        inputEvent.isKeyRelease(gui::KeyCode::KEY_ENTER)) {
                        switchWindow(*getPreviousWindow());
                        return true;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        return false;
    }

    // Empty: do not start idleTimer on application run
    void ApplicationBellOnBoarding::onStart()
    {}
} // namespace app
