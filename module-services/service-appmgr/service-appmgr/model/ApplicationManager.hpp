// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationHandle.hpp"
#include "ApplicationsRegistry.hpp"
#include "ActionsRegistry.hpp"

#include <module-apps/Application.hpp>
#include <module-apps/ApplicationLauncher.hpp>

#include <service-appmgr/messages/Message.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Timer.hpp>
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
        void startBackgroundApplications();
        void rebuildActiveApplications();
        void suspendSystemServices();
        auto closeServices() -> bool;
        auto closeApplications() -> bool;
        auto closeApplicationsOnUpdate() -> bool;
        void closeService(const std::string &name);
        void closeApplication(ApplicationHandle *application);

        // Message handlers
        void registerMessageHandlers();
        bool handleAction(ActionEntry &action);
        void handleActionRequest(ActionRequest *actionMsg);
        auto handleHomeAction(ActionEntry &action) -> bool;
        auto handleLaunchAction(ActionEntry &action) -> bool;
        auto handleCloseSystem() -> bool;
        auto handleCustomAction(ActionEntry &action) -> bool;
        auto handleSwitchApplication(SwitchRequest *msg, bool closeCurrentlyFocusedApp = true) -> bool;
        auto handleCloseConfirmation(CloseConfirmation *msg) -> bool;
        auto handleSwitchConfirmation(SwitchConfirmation *msg) -> bool;
        auto handleSwitchBack(SwitchBackRequest *msg) -> bool;
        auto handleInitApplication(ApplicationInitialised *msg) -> bool;
        auto handleDisplayLanguageChange(DisplayLanguageChangeRequest *msg) -> bool;
        auto handleInputLanguageChange(InputLanguageChangeRequest *msg) -> bool;
        auto handleAutomaticDateAndTimeChange(AutomaticDateAndTimeIsOnChangeRequest *msg) -> bool;
        auto handleAutomaticTimeZoneChange(AutomaticTimeZoneIsOnChangeRequest *msg) -> bool;
        auto handleTimeFormatChange(TimeFormatChangeRequest *msg) -> bool;
        auto handleDateFormatChange(DateFormatChangeRequest *msg) -> bool;
        auto handleSetOsUpdateVersionChange(SetOsUpdateVersion *msg) -> bool;
        auto handlePowerSavingModeInit() -> bool;
        auto handleMessageAsAction(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>;
        /// handles dom request by passing this request to application which should provide the dom
        auto handleDOMRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>;

        void requestApplicationClose(ApplicationHandle &app, bool isCloseable);
        void onApplicationSwitch(ApplicationHandle &app,
                                 std::unique_ptr<gui::SwitchData> &&data,
                                 std::string targetWindow);
        void onApplicationSwitchToPrev(ApplicationHandle &previousApp, std::unique_ptr<gui::SwitchData> &&data);
        void onApplicationInitialised(ApplicationHandle &app, StartInBackground startInBackground);
        void onApplicationInitFailure(ApplicationHandle &app);
        auto onSwitchConfirmed(ApplicationHandle &app) -> bool;
        void onLaunchFinished(ApplicationHandle &app);
        auto onCloseConfirmed(ApplicationHandle &app) -> bool;
        void onPhoneLocked();

        ApplicationName rootApplicationName;
        ActionsRegistry actionsRegistry;
        std::unique_ptr<sys::Timer> blockingTimer; //< timer to count time from last user's activity. If it reaches time
                                                   // defined in settings database application
                                                   // manager is sending signal to power manager and changing window to
                                                   // the desktop window in the blocked state.
        std::unique_ptr<sys::Timer> shutdownDelay;
        std::unique_ptr<settings::Settings> settings;
        std::unique_ptr<sys::phone_modes::Observer> phoneModeObserver;
        void displayLanguageChanged(std::string value);
        void lockTimeChanged(std::string value);
        void inputLanguageChanged(std::string value);
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
