// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationHandle.hpp"
#include "ApplicationsRegistry.hpp"
#include "ActionsRegistry.hpp"
#include "ApplicationStack.hpp"
#include "OnActionPolicy.hpp"
#include <service-appmgr/messages/Message.hpp>

#include <apps-common/Application.hpp>
#include <apps-common/ApplicationLauncher.hpp>

#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Timers/TimerHandle.hpp>
#include <PhoneModes/Observer.hpp>
#include <SwitchData.hpp>

#include <deque>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <service-db/DBServiceName.hpp>
#include <service-db/Settings.hpp>
#include <service-gui/Common.hpp>
#include <service-eink/Common.hpp>

#include <notifications/NotificationProvider.hpp>
#include <apps-common/locks/handlers/PhoneLockHandler.hpp>
#include <apps-common/locks/handlers/SimLockHandler.hpp>
#include <apps-common/notifications/NotificationsHandler.hpp>
#include <apps-common/notifications/NotificationsConfiguration.hpp>

namespace app
{
    class ApplicationLauncher;
    namespace manager
    {
        class APMAction;
        class APMChangeLanguage;
        class APMConfirmClose;
        class APMConfirmSwitch;
        class APMRegister;
        class APMSwitch;
        class APMSwitchPrevApp;
        class GetAutoLockTimeoutRequest;
        class SetAutoLockTimeoutRequest;
    } // namespace manager
} // namespace app

namespace app::manager
{
    class ApplicationManagerBase
    {
      public:
        enum class State
        {
            Running,
            AwaitingFocusConfirmation,
            AwaitingCloseConfirmation,
            AwaitingLostFocusConfirmation
        };

        explicit ApplicationManagerBase(std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers);
        virtual ~ApplicationManagerBase() = default;

        auto getFocusedApplication() const noexcept -> ApplicationHandle *;
        auto getLaunchingApplication() const noexcept -> ApplicationHandle *;
        auto getPreviousApplication() const noexcept -> ApplicationHandle *;
        auto getApplication(const ApplicationName &name) const noexcept -> ApplicationHandle *;
        auto getStackedApplications() noexcept -> std::vector<ApplicationHandle *>;
        auto getApplications() const noexcept -> const ApplicationsContainer &
        {
            return applications.getAll();
        }
        auto isApplicationCloseable(const ApplicationHandle *app) const noexcept -> bool;

        void setState(State _state) noexcept;
        auto getState() const noexcept -> State
        {
            return state;
        }

      protected:
        ApplicationsRegistry applications;
        ApplicationStack stack;

      private:
        State state = State::Running;
    };

    class ApplicationManager : public sys::Service, private ApplicationManagerBase
    {
      public:
        static constexpr auto ServiceName = "ApplicationManager";

        ApplicationManager(const ApplicationName &serviceName,
                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                           const ApplicationName &_rootApplicationName);

        auto InitHandler() -> sys::ReturnCodes override;
        auto DeinitHandler() -> sys::ReturnCodes override;
        auto ProcessCloseReason(sys::CloseReason closeReason) -> void override;
        auto SwitchPowerModeHandler(const sys::ServicePowerMode mode) -> sys::ReturnCodes override;
        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::MessagePointer override;

      private:
        auto startApplication(ApplicationHandle &app) -> bool;
        void startBackgroundApplications();
        void rebuildActiveApplications();
        void suspendSystemServices();
        void closeNoLongerNeededApplications();
        auto closeApplications() -> bool;
        auto closeApplicationsOnUpdate() -> bool;
        void closeApplication(ApplicationHandle *application);

        // Message handlers
        void registerMessageHandlers();
        auto handleAction(ActionEntry &action) -> ActionProcessStatus;
        void handleActionRequest(ActionRequest *actionMsg);
        auto handleHomeAction(ActionEntry &action) -> ActionProcessStatus;
        auto resolveHomeWindow() -> std::string;
        auto handleOnBoardingFinalize() -> sys::MessagePointer;
        auto checkOnBoarding() -> bool;
        auto resolveHomeApplication() -> std::string;
        auto handleLaunchAction(ActionEntry &action) -> ActionProcessStatus;
        auto handleActionOnFocusedApp(ActionEntry &action) -> ActionProcessStatus;
        auto handlePhoneModeChangedAction(ActionEntry &action) -> ActionProcessStatus;
        void handlePhoneModeChanged(sys::phone_modes::PhoneMode phoneMode);
        void handleTetheringChanged(sys::phone_modes::Tethering tethering);
        void changePhoneMode(sys::phone_modes::PhoneMode phoneMode, const ApplicationHandle *app);
        auto handleCustomAction(ActionEntry &action) -> ActionProcessStatus;
        auto handleCustomActionOnBackgroundApp(ApplicationHandle *app, ActionEntry &action) -> ActionProcessStatus;
        auto handleSwitchApplication(SwitchRequest *msg, bool closeCurrentlyFocusedApp = true) -> bool;
        auto handleCloseConfirmation(CloseConfirmation *msg) -> bool;
        auto handleSwitchConfirmation(SwitchConfirmation *msg) -> bool;
        auto handleSwitchBack(SwitchBackRequest *msg) -> bool;
        auto handleInitApplication(ApplicationInitialised *msg) -> bool;
        auto handleDisplayLanguageChange(DisplayLanguageChangeRequest *msg) -> bool;
        auto handleInputLanguageChange(InputLanguageChangeRequest *msg) -> bool;
        auto handleDBResponse(db::QueryResponse *msg) -> bool;
        auto handlePowerSavingModeInit() -> bool;
        auto handleMessageAsAction(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>;
        auto handleDeveloperModeRequest(sys::Message *request) -> sys::MessagePointer;
        /// handles dom request by passing this request to application which should provide the dom
        auto handleDOMRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>;
        void handleStart(StartAllowedMessage *msg);
        auto handleAutoLockGetRequest(GetAutoLockTimeoutRequest *request) -> std::shared_ptr<sys::ResponseMessage>;
        auto handleAutoLockSetRequest(SetAutoLockTimeoutRequest *request) -> std::shared_ptr<sys::ResponseMessage>;

        void requestApplicationClose(ApplicationHandle &app, bool isCloseable);
        void onApplicationSwitch(ApplicationHandle &nextApp,
                                 std::unique_ptr<gui::SwitchData> &&data,
                                 std::string targetWindow);
        void onApplicationSwitchToPrev(ApplicationHandle &previousApp, std::unique_ptr<gui::SwitchData> &&data);
        void onApplicationInitialised(ApplicationHandle &app, StartInBackground startInBackground);
        void onApplicationInitFailure(ApplicationHandle &app);
        auto onSwitchConfirmed(ApplicationHandle &app) -> bool;
        void onLaunchFinished(ApplicationHandle &app);
        auto onCloseConfirmed(ApplicationHandle &app) -> bool;
        /// @brief method is called on auto-locking timer tick event (blockTimer)
        /// @detailed It sends AutoLock action to ApplicationDesktop to lock the screen.
        /// @note AutoLock action is sent only if following conditions are met:
        ///  - tethering is off
        ///  - focused application is not preventing AutoLock
        void onPhoneLocked();

        ApplicationName rootApplicationName;
        ActionsRegistry actionsRegistry;
        OnActionPolicy actionPolicy;

        sys::TimerHandle autoLockTimer; //< auto-lock timer to count time from last user's activity.
                                        // If it reaches time defined in settings database application
                                        // manager is sending signal to Application Desktop in order to
                                        // lock screen.
        std::shared_ptr<settings::Settings> settings;
        std::shared_ptr<sys::phone_modes::Observer> phoneModeObserver;

        locks::PhoneLockHandler phoneLockHandler;
        locks::SimLockHandler simLockHandler;

        notifications::NotificationsConfiguration notificationsConfig;
        notifications::NotificationsHandler notificationsHandler;
        notifications::NotificationProvider notificationProvider;

        void displayLanguageChanged(std::string value);
        void lockTimeChanged(std::string value);
        void inputLanguageChanged(std::string value);

        void processKeypadBacklightState(bsp::keypad_backlight::State keypadLightState);
    };
} // namespace app::manager

namespace sys
{
    template <> struct ManifestTraits<app::manager::ApplicationManager>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = app::manager::ApplicationManager::ServiceName;
            manifest.dependencies = {service::name::db, service::name::gui, service::name::eink};
            return manifest;
        }
    };
} // namespace sys
