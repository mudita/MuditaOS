// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>      // for unique_ptr, allocator
#include <string>      // for string
#include <deque>       // for deque
#include <string_view> // for string_view
#include <vector>      // for vector

#include "Application.hpp"         // for Application, Application::State
#include "ApplicationLauncher.hpp" // for ApplicationLauncher
#include "module-services/service-appmgr/messages/Message.hpp"
#include "Service/Common.hpp"  // for ReturnCodes, ServicePowerMode
#include "Service/Message.hpp" // for Message_t, DataMessage (ptr only), ResponseMessage (ptr only)
#include "Service/Service.hpp" // for Service
#include "Service/Timer.hpp"   // for Timer
#include "SettingsRecord.hpp"  // for SettingsRecord
#include "SwitchData.hpp"      // for SwitchData

#include "ApplicationHandle.hpp"
#include "ApplicationsRegistry.hpp"

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
        static inline const std::string ServiceName = "ApplicationManager";

        ApplicationManager(const ApplicationName &serviceName,
                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                           const ApplicationName &_rootApplicationName);

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
        auto handleAction(ActionRequest *actionMsg) -> bool;
        auto handleLaunchAction(ApplicationLaunchData *launchParams) -> bool;
        auto handleSwitchApplication(SwitchRequest *msg) -> bool;
        auto handleCloseConfirmation(CloseConfirmation *msg) -> bool;
        auto handleSwitchConfirmation(SwitchConfirmation *msg) -> bool;
        auto handleSwitchBack(SwitchBackRequest *msg) -> bool;
        auto handleInitApplication(ApplicationInitialisation *msg) -> bool;
        auto handleLanguageChange(LanguageChangeRequest *msg) -> bool;
        auto handlePowerSavingModeInit() -> bool;

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
