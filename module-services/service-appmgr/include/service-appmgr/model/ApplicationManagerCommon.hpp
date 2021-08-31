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

#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Timers/TimerHandle.hpp>

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
#include <apps-common/notifications/NotificationsHandler.hpp>
#include <apps-common/notifications/NotificationsConfiguration.hpp>

namespace app
{
    class ApplicationLauncher;
    namespace manager
    {
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

    class ApplicationManagerCommon : public sys::Service, protected ApplicationManagerBase
    {
      public:
        ApplicationManagerCommon(const ApplicationName &serviceName,
                                 std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                                 const ApplicationName &_rootApplicationName);

        auto InitHandler() -> sys::ReturnCodes override;
        auto DeinitHandler() -> sys::ReturnCodes override;
        auto ProcessCloseReason(sys::CloseReason closeReason) -> void override;
        auto SwitchPowerModeHandler(const sys::ServicePowerMode mode) -> sys::ReturnCodes override;
        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::MessagePointer override;

      protected:
        virtual auto handleAction(ActionEntry &action) -> ActionProcessStatus;
        auto handleActionOnActiveApps(ActionEntry &action) -> ActionProcessStatus;
        auto handleMessageAsAction(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>;
        virtual auto startApplication(ApplicationHandle &app) -> bool;
        virtual auto resolveHomeWindow() -> std::string;
        virtual auto resolveHomeApplication() -> std::string
        {
            return rootApplicationName;
        }
        auto checkOnBoarding() -> bool;
        virtual void registerMessageHandlers();
        auto handleSwitchApplication(SwitchRequest *msg, bool closeCurrentlyFocusedApp = true) -> bool;
        virtual void handleStart(StartAllowedMessage *msg);

        ApplicationName rootApplicationName;
        ActionsRegistry actionsRegistry;
        std::shared_ptr<settings::Settings> settings;

      private:
        void startPendingApplicationOnCurrentClose();
        void rebuildActiveApplications();
        void suspendSystemServices();
        void closeNoLongerNeededApplications();
        auto closeApplications() -> bool;
        void closeApplication(ApplicationHandle *application);

        // Message handlers
        void handleActionRequest(ActionRequest *actionMsg);
        auto handleHomeAction(ActionEntry &action) -> ActionProcessStatus;
        auto handleOnBoardingFinalize() -> sys::MessagePointer;
        auto handleLaunchAction(ActionEntry &action) -> ActionProcessStatus;
        auto handleActionOnFocusedApp(ActionEntry &action) -> ActionProcessStatus;
        auto handleCustomAction(ActionEntry &action) -> ActionProcessStatus;
        auto handleCustomActionOnBackgroundApp(ApplicationHandle *app, ActionEntry &action) -> ActionProcessStatus;
        void handleFinalizingClose(FinalizingClose *msg);
        auto handleCloseConfirmation(CloseConfirmation *msg) -> bool;
        auto handleSwitchConfirmation(SwitchConfirmation *msg) -> bool;
        auto handleSwitchBack(SwitchBackRequest *msg) -> bool;
        auto handleInitApplication(ApplicationInitialised *msg) -> bool;
        auto handleDisplayLanguageChange(DisplayLanguageChangeRequest *msg) -> bool;
        auto handleInputLanguageChange(InputLanguageChangeRequest *msg) -> bool;
        auto handlePowerSavingModeInit() -> bool;
        virtual auto handleDeveloperModeRequest(sys::Message *request) -> sys::MessagePointer;
        /// handles dom request by passing this request to application which should provide the dom
        auto handleDOMRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>;

        void requestApplicationClose(ApplicationHandle &app, bool isCloseable);
        void onApplicationSwitch(ApplicationHandle &nextApp,
                                 std::unique_ptr<gui::SwitchData> &&data,
                                 std::string targetWindow);
        void onApplicationSwitchToPrev(ApplicationHandle &previousApp, std::unique_ptr<gui::SwitchData> &&data);
        void onApplicationInitialised(ApplicationHandle &app, StartInBackground startInBackground);
        void onApplicationInitFailure(ApplicationHandle &app);
        auto onSwitchConfirmed(ApplicationHandle &app) -> bool;
        void onLaunchFinished(ApplicationHandle &app);
        void onFinalizingClose();
        auto onCloseConfirmed(ApplicationHandle &app) -> bool;

        OnActionPolicy actionPolicy;

        void displayLanguageChanged(std::string value);
        void inputLanguageChanged(std::string value);
    };
} // namespace app::manager
