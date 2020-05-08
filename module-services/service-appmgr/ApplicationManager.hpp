/*
 * @file ApplicationManager.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SERVICES_SERVICE_APPMGR_APPLICATIONMANAGER_HPP_
#define MODULE_SERVICES_SERVICE_APPMGR_APPLICATIONMANAGER_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include <map>
#include <string.h>
#include "Application.hpp"
#include "SystemManager/SystemManager.hpp"
#include "messages/APMMessage.hpp"
#include "i18/i18.hpp"

inline uint32_t default_application_locktime = 30000;

namespace sapm
{

    class ApplicationDescription
    {
      public:
        ApplicationDescription(std::unique_ptr<app::ApplicationLauncher> launcher);
        virtual ~ApplicationDescription()
        {}
        // name of the application. It's used to find proper application during switching
        std::string name()
        {
            if (launcher) {
                return launcher->getName();
            }
            else {
                return "NONE";
            }
        }
        // launcher to application to the application's start function
        std::unique_ptr<app::ApplicationLauncher> launcher = nullptr;
        // informs application manager that this application temporary musn't be closed.
        // This flag is used to prevent application closing when application is closeable and there is incoming call.
        // This flag is also used when closeable application is on front and there is a timeout to block the
        // applicatioin.
        bool blockClosing = false;
        // switching data stored when application manager had to run init function
        std::unique_ptr<gui::SwitchData> switchData = nullptr;
        std::string switchWindow                    = "";

        // prevents from blocking the system
        bool preventsLocking()
        {
            if (launcher != nullptr) {
                return launcher->isBlocking();
            }
            else {
                return false;
            }
        }

        // informs if it is possible to close application when it looses focus.
        bool closeable()
        {
            if (launcher != nullptr) {
                return launcher->isCloseable();
            }
            return false;
        }

        app::Application::State getState()
        {
            if ((launcher == nullptr) || (launcher->handle == nullptr)) {
                return app::Application::State::NONE;
            }
            else {
                return launcher->handle->getState();
            }
        }

        void setState(app::Application::State st)
        {
            if ((launcher == nullptr) || (launcher->handle == nullptr)) {
                LOG_ERROR("No %s", launcher == nullptr ? "launcher" : "handle");
            }
            else {
                launcher->handle->setState(st);
            }
        }
    };

    /// this is only to force usage of get/set methods in ApplicatioManager
    class VirtualAppManager
    {
      public:
        enum class State
        {
            IDLE,
            CLOSING_PREV_APP,
            WAITING_CLOSE_CONFIRMATION,
            STARTING_NEW_APP,
            WAITING_NEW_APP_REGISTRATION,
            WAITING_LOST_FOCUS_CONFIRMATION,
            WAITING_GET_FOCUS_CONFIRMATION
        };

        VirtualAppManager(std::vector<std::unique_ptr<app::ApplicationLauncher>> &launchers);
        virtual ~VirtualAppManager();

      private:
        State state = State::IDLE;
        std::list<ApplicationDescription *> applications;

      public:
        std::list<std::string> appStack;
        State getState()
        {
            return state;
        }
        static const char *stateStr(State st);
        ApplicationDescription *appFront();
        ApplicationDescription *appGet(const std::string &name);
        /// set application as first on the applications vector
        bool appMoveFront(ApplicationDescription *app);
        /// get previous visible app - one on FOREGROUND
        ApplicationDescription *appPrev();
        void setState(State st);
        std::list<ApplicationDescription *> &getApps();
        void debug_log_app_list();
    };

    class ApplicationManager : public sys::Service, public VirtualAppManager
    {

        SettingsRecord settings;

        sys::SystemManager *systemManager;

        //
        std::unique_ptr<utils::i18> i18;

        // application that currently has focus. This means that is has rights to display screens and receive keyboard
        // events.
        std::string focusApplicationName = "";
        // after loosing focus application becomes previous application and this is its name
        std::string previousApplicationName = "";
        // name of the application scheduled for launching
        std::string launchApplicationName = "";
        // timer to count time from last user's activity. If it reaches time defined in settings database application
        // manager is sending signal to power manager and changing window to the desktop window in the blocked state.
        uint32_t blockingTimerID = 0;

        // tries to switch the application
        bool handleSwitchApplication(APMSwitch *msg);
        //	bool handleSwitchApplicationWithData( APMSwitchWithData* msg);
        bool handleCloseConfirmation(APMConfirmClose *msg);
        bool handleSwitchConfirmation(APMConfirmSwitch *msg);
        bool handleSwitchPrevApplication(APMSwitchPrevApp *msg);
        bool handleRegisterApplication(APMRegister *msg);
        bool handleLanguageChange(sapm::APMChangeLanguage *msg);
        bool handlePowerSavingModeInit();
        /**
         * @brief Closes all running applications.
         */
        bool startApplication(const std::string &appName);
        /**
         * @brief Closes eink and gui services.
         */
        bool closeApplications();
        bool closeServices();

      public:
        ApplicationManager(const std::string &name,
                           sys::SystemManager *sysmgr,
                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &launchers);
        virtual ~ApplicationManager();

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        // Invoked when timer ticked
        void TickHandler(uint32_t id) override;

        // Invoked during initialization
        sys::ReturnCodes InitHandler() override;

        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

        /**
         * @brief Sends request to application manager to switch from current application to specific window in
         * application with specified name .
         */
        static bool messageSwitchApplication(sys::Service *sender,
                                             const std::string &applicationName,
                                             const std::string &windowName,
                                             std::unique_ptr<gui::SwitchData> data = nullptr);

        static bool messageSwitchSpecialInput(sys::Service *sender, std::unique_ptr<gui::SwitchSpecialChar> data);
        /**
         * @brief Sends request to application manager to switch from current application to specific window in
         * application with specified name. Allows sending data to destination application.
         */
        //    static bool messageSwitchApplicationWithData( sys::Service* sender, const std::string& applicationName,
        //    const std::string& windowName, std::unique_ptr<app::SwitchData>& switchData );
        /**
         * @Brief Function allows sending confirmation to the switch request. This is sent both by application that
         * gains and looses focus.
         */
        static bool messageConfirmSwitch(sys::Service *sender);
        /**
         * @brief Function allows application to confirm close request.
         */
        static bool messageConfirmClose(sys::Service *sender);
        /**
         * @brief Allows requesting Application Manager to run previous application.
         */
        static bool messageSwitchPreviousApplication(sys::Service *sender,
                                                     std::unique_ptr<APMSwitchPrevApp> msg = nullptr);
        /**
         * @brief Sends information from application to manager about result of application's init function.
         * If successful message will contain name and true value, otherwise false value will be transmitted.
         */
        static bool messageRegisterApplication(sys::Service *sender, const bool &status, const bool &startBackground);
        /**
         * @brief Sends message to application manager to inform it about change of the phone's language performed by
         * the user.
         */
        static bool messageChangeLanguage(sys::Service *sender, utils::Lang language);
        /**
         * @brief Sends message to application manager that it should close itself and as a result.
         */
        static bool messageCloseApplicationManager(sys::Service *sender);
        /**
         * @brief Sends message to inform Application Manager to reset timer responsible for blocking phone
         */
        static bool messagePreventBlocking(sys::Service *sender);
        /**
         * @brief Sends message to Application Manager. This will initialize procedure of switching to power saving
         * mode.
         */
        static bool messageInitPowerSaveMode(sys::Service *sender);

        /**
         * @brief Sends message to Application Manager.
         *
         * \returns true if app manager running and app is registered == running, false othervise
         */
        static bool appRunning(sys::Service *sender, const std::string &name);
    };

} /* namespace sapm */

#endif /* MODULE_SERVICES_SERVICE_APPMGR_APPLICATIONMANAGER_HPP_ */
