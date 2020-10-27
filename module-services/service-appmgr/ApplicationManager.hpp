// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <string>

#include "Application.hpp"
#include "ApplicationLauncher.hpp"
#include "messages/APMMessage.hpp"

namespace app
{
    class ApplicationLauncher;
}

namespace app::manager
{
    class ApplicationHandle
    {
      public:
        static inline constexpr std::string_view InvalidAppName{"NONE"};

        using State = app::Application::State;
        using Name  = std::string;

        explicit ApplicationHandle(std::unique_ptr<app::ApplicationLauncher> &&_launcher);

        void setState(State state) noexcept;
        void run(sys::Service *caller);
        void runInBackground(sys::Service *caller);

        auto name() const -> Name;
        auto state() const noexcept -> State;
        auto preventsBlocking() const noexcept -> bool;
        auto closeable() const noexcept -> bool;
        auto started() const noexcept -> bool;

        std::unique_ptr<app::ApplicationLauncher> launcher; // Handle to the application's start function.
        std::unique_ptr<gui::SwitchData> switchData;
        std::string switchWindow;
        bool blockClosing =
            false; //< Informs the application manager that this application mustn't be closed temporarily.
                   // This flag is used to prevent application closing when application is closeable and there is
                   // incoming call. This flag is also used when closeable application is on front and there is a
                   // timeout to block the application.
    };

    class ApplicationManagerBase
    {
      public:
        using ApplicationsContainer = std::vector<std::unique_ptr<ApplicationHandle>>;
        using ApplicationsStack     = std::deque<ApplicationHandle::Name>;
        enum class State
        {
            Running,
            AwaitingFocusConfirmation,
            AwaitingCloseConfirmation,
            AwaitingLostFocusConfirmation
        };

        explicit ApplicationManagerBase(std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers);
        virtual ~ApplicationManagerBase() = default;

        void pushApplication(const ApplicationHandle::Name &name);
        void popApplication();
        void clearStack();

        auto getFocusedApplication() const noexcept -> ApplicationHandle *;
        auto getLaunchingApplication() const noexcept -> ApplicationHandle *;
        auto getPreviousApplication() const noexcept -> ApplicationHandle *;
        auto getApplication(const ApplicationHandle::Name &name) const noexcept -> ApplicationHandle *;
        auto getApplications() const noexcept -> const ApplicationsContainer &
        {
            return applications;
        }

        void setState(State _state) noexcept;
        auto getState() const noexcept -> State
        {
            return state;
        }

      private:
        State state = State::Running;
        ApplicationsContainer applications;
        ApplicationsStack stack;
    };

    class ApplicationManager : public sys::Service, private ApplicationManagerBase
    {
      public:
        static inline const std::string ServiceName = "ApplicationManager";

        ApplicationManager(const std::string &serviceName,
                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                           const ApplicationHandle::Name &_rootApplicationName);

        auto InitHandler() -> sys::ReturnCodes override;
        auto DeinitHandler() -> sys::ReturnCodes override;
        auto SwitchPowerModeHandler(const sys::ServicePowerMode mode) -> sys::ReturnCodes override;
        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::Message_t override;

      private:
        auto startApplication(ApplicationHandle &app) -> bool;
        void startSystemServices();
        void startBackgroundApplications();
        void rebuildActiveApplications();
        void suspendSystemServices();
        auto closeServices() -> bool;
        auto closeApplications() -> bool;
        void closeService(const std::string &name);

        // Message handlers
        void registerMessageHandlers();
        auto handleAction(APMAction *actionMsg) -> bool;
        auto handleSwitchApplication(APMSwitch *msg) -> bool;
        auto handleCloseConfirmation(APMConfirmClose *msg) -> bool;
        auto handleSwitchConfirmation(APMConfirmSwitch *msg) -> bool;
        auto handleSwitchBack(APMSwitchPrevApp *msg) -> bool;
        auto handleRegisterApplication(APMRegister *msg) -> bool;
        auto handleLanguageChange(APMChangeLanguage *msg) -> bool;
        auto handlePowerSavingModeInit() -> bool;

        void requestApplicationClose(ApplicationHandle &app, bool isCloseable);
        void onApplicationSwitch(ApplicationHandle &app,
                                 std::unique_ptr<gui::SwitchData> &&data,
                                 std::string targetWindow);
        void onApplicationSwitchToPrev(ApplicationHandle &previousApp,
                                       std::unique_ptr<gui::SwitchData> &&data,
                                       std::string targetWindow = {});
        void onApplicationRegistered(ApplicationHandle &app, bool startInBackground);
        void onApplicationRegistrationFailure(ApplicationHandle &app);
        auto onSwitchConfirmed(ApplicationHandle &app) -> bool;
        auto onCloseConfirmed(ApplicationHandle &app) -> bool;
        void onPhoneLocked();

        ApplicationHandle::Name rootApplicationName;
        SettingsRecord settings;
        std::unique_ptr<sys::Timer> blockingTimer; //< timer to count time from last user's activity. If it reaches time
                                                   // defined in settings database application
                                                   // manager is sending signal to power manager and changing window to
                                                   // the desktop window in the blocked state.
    };
} // namespace app::manager
