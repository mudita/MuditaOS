// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationHandle.hpp"
#include "ApplicationsRegistry.hpp"

#include <module-apps/Application.hpp>
#include <module-apps/ApplicationLauncher.hpp>

#include <service-appmgr/messages/Message.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Timer.hpp>
#include <SettingsRecord.hpp>
#include <SwitchData.hpp>

#include <deque>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

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

        void pushApplication(const ApplicationName &name);
        void popApplication();
        void clearStack();

        auto getFocusedApplication() const noexcept -> ApplicationHandle *;
        auto getLaunchingApplication() const noexcept -> ApplicationHandle *;
        auto getPreviousApplication() const noexcept -> ApplicationHandle *;
        auto getApplication(const ApplicationName &name) const noexcept -> ApplicationHandle *;
        auto getApplications() const noexcept -> const ApplicationsContainer &
        {
            return applications.getAll();
        }

        void setState(State _state) noexcept;
        auto getState() const noexcept -> State
        {
            return state;
        }

      protected:
        ApplicationsRegistry applications;

      private:
        using ApplicationsStack = std::deque<ApplicationName>;

        State state = State::Running;
        ApplicationsStack stack;
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
        auto SwitchPowerModeHandler(const sys::ServicePowerMode mode) -> sys::ReturnCodes override;
        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::MessagePointer override;

      private:
        auto startApplication(ApplicationHandle &app) -> bool;
        void startSystemServices();
        void startBackgroundApplications();
        void rebuildActiveApplications();
        void suspendSystemServices();
        auto closeServices() -> bool;
        auto closeApplications() -> bool;
        void closeService(const std::string &name);
        void closeApplication(ApplicationHandle *application);

        // Message handlers
        void registerMessageHandlers();
        auto handleAction(ActionRequest *actionMsg) -> bool;
        auto handleLaunchAction(ApplicationLaunchData *launchParams) -> bool;
        auto handleSwitchApplication(SwitchRequest *msg) -> bool;
        auto handleCloseConfirmation(CloseConfirmation *msg) -> bool;
        auto handleSwitchConfirmation(SwitchConfirmation *msg) -> bool;
        auto handleSwitchBack(SwitchBackRequest *msg) -> bool;
        auto handleInitApplication(ApplicationInitialised *msg) -> bool;
        auto handleDisplayLanguageChange(DisplayLanguageChangeRequest *msg) -> bool;
        auto handleInputLanguageChange(InputLanguageChangeRequest *msg) -> bool;
        auto handlePowerSavingModeInit() -> bool;
        auto makeConvertibleToActionHandler() -> sys::MessageHandler;

        void requestApplicationClose(ApplicationHandle &app, bool isCloseable);
        void onApplicationSwitch(ApplicationHandle &app,
                                 std::unique_ptr<gui::SwitchData> &&data,
                                 std::string targetWindow);
        void onApplicationSwitchToPrev(ApplicationHandle &previousApp,
                                       std::unique_ptr<gui::SwitchData> &&data,
                                       std::string targetWindow = {});
        void onApplicationInitialised(ApplicationHandle &app, StartInBackground startInBackground);
        void onApplicationInitFailure(ApplicationHandle &app);
        auto onSwitchConfirmed(ApplicationHandle &app) -> bool;
        auto onCloseConfirmed(ApplicationHandle &app) -> bool;
        void onPhoneLocked();

        ApplicationName rootApplicationName;
        SettingsRecord settings;
        std::unique_ptr<sys::Timer> blockingTimer; //< timer to count time from last user's activity. If it reaches time
                                                   // defined in settings database application
                                                   // manager is sending signal to power manager and changing window to
                                                   // the desktop window in the blocked state.
        // Temporary solution - to be replaced with ActionsMiddleware.
        std::tuple<ApplicationName, actions::ActionId, actions::ActionParamsPtr> pendingAction;
    };
} // namespace app::manager
