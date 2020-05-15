/*
 * @file Application.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_HPP_
#define MODULE_APPS_APPLICATION_HPP_

#include <map>
// module-gui
#include "gui/Common.hpp"
//#include "gui/widgets/Window.hpp"
#include "gui/input/Translator.hpp"
// module-sys
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "Service/Common.hpp"
#include "SystemManager/SystemManager.hpp"
// module-db
#include "Interface/SettingsRecord.hpp"

#include "SwitchData.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include "windows/AppWindow.hpp"

namespace gui
{
    class AppWindow;
};

namespace app
{

    class Application;

    class AppTimer // this should inherit from ServiceTimer, but *bodge*
    {
      private:
        uint32_t id                    = 0; // let's say 0 indicates not initalized timer
        std::function<void()> callback = nullptr;
        Application *parent            = nullptr;

        void registerCallback(std::function<void()>);
        AppTimer();

      public:
        AppTimer(Application *parent, uint32_t id, std::function<void()> callback, const std::string &name);
        ~AppTimer();
        void runCallback();
        uint32_t getID();
        static uint32_t getNextUniqueID();
        // timer controls:
        void restart();
        void stop();
        bool operator==(const AppTimer &rhs) const;
        bool operator==(const uint32_t &rhs) const;
    };

    inline auto msgHandled() -> sys::Message_t
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    inline auto msgNotHandled() -> sys::Message_t
    {
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }

    /// This is template for creating new applications. Main difference between Application and service is that:
    /// 1. Application has access to GUI and Input
    /// 2. Application lifetime is managed with sapm::ApplicationManager
    class Application : public sys::Service
    {
      public:
        /// state in which application is right now
        /// @note do not mistake with application manager state
        enum class State
        {
            /// Application doesn't exist
            NONE,
            /// Application: Object has been created and underlying service is waiting to execute init handler method.
            /// Application Manager: Launcher for the application has been provided. Application can be started using
            /// provided launcher. The other possibility is that Appication Manager received CLOSING_FINISHED message.
            DEACTIVATED,
            /// Application: Set after entering the init handler of the application. In this state application will
            /// request in a blocking way the db configuration of the phone. Before exiting the init handler application
            /// must send APP_REGISTER message to Application Manager. Application Manager: Initialization is triggered
            /// by the switch message sent by other application to the application manager. Launcher for the application
            /// has been executed upon receiving switch command. Optional switch data has been saved and it will be
            /// provided when application manager receives registration message.
            INITIALIZING,
            /// Application manager sent variant of switch command to the selected application and it's now waiting for
            /// confirmation from the application
            ACTIVATING,
            /// Application is working and has focus and can render
            ACTIVE_FORGROUND,
            /// Applicatino lost focus but it is still working
            ACTIVE_BACKGROUND,
            /// Application: Close request message has been received. As a response application must send close request
            /// acknowledge message. Application must start closing all workers and releasing resources. After all
            /// workers are closed and resources released application sends to application manager CLOSING_FINISHED
            /// message Application Manager sent signal to application that it should be closed. Application must
            /// perform all necessary cleanup and request System Manager to close it.
            DEACTIVATING
        };
        /// c_str() function for Application::State
        static const char *stateStr(State st);

      private:
        State state = State::DEACTIVATED;

        sys::Message_t handleSignalStrengthUpdate(sys::DataMessage *msgl);
        sys::Message_t handleInputEvent(sys::DataMessage *msgl);
        sys::Message_t handleKBDKeyEvent(sys::DataMessage *msgl);
        sys::Message_t handleBatteryLevel(sys::DataMessage *msgl);
        sys::Message_t handleChargerPlugged(sys::DataMessage *msgl);
        sys::Message_t handleMinuteUpdated(sys::DataMessage *msgl);
        sys::Message_t handleApplicationSwitch(sys::DataMessage *msgl);
        sys::Message_t handleSwitchWindow(sys::DataMessage *msgl);
        sys::Message_t handleAppClose(sys::DataMessage *msgl);
        sys::Message_t handleAppRebuild(sys::DataMessage *msgl);
        sys::Message_t handleAppRefresh(sys::DataMessage *msgl);
        sys::Message_t handleSIMMessage(sys::DataMessage *msgl);

      public:
        std::list<uint32_t> timerIDs;
        std::list<AppTimer> appTimers; // @TODO decide on type
        AppTimer longPressTimer;
        Application(std::string name,
                    std::string parent            = "",
                    bool startBackground          = false,
                    uint32_t stackDepth           = 4096,
                    sys::ServicePriority priority = sys::ServicePriority::Idle);
        virtual ~Application();

        Application::State getState();
        void setState(State st);

        /// @defgroup AppTimers Application timers
        /// @note Please mind that timers are from Service and implementation in service should be revritten to send
        /// notify instead of calling callback
        ///       Right now timers can create races.
        /// @{
        /// Method to register callback function to be run on timer.
        AppTimer CreateAppTimer(TickType_t interval,
                                bool isPeriodic,
                                std::function<void()> callback,
                                const std::string &name = "");
        /// Remove previousy registered AppTimer by object
        void DeleteTimer(AppTimer &timer);
        /// Remove previousy registered AppTimer by id
        void DeleteTimer(uint32_t id);
        /// @}

        /// Method responsible for rendering currently active window.
        /// 1. queries for static data for all windows form Store (i.e. battery level, sim card level)
        /// 2. loads rendering commands and send them to GUI renderer (sgui::ServiceGUI)
        void render(gui::RefreshModes mode);

        /// Method responsible for setting application to the state where incoming user input is blocked
        void blockEvents(bool isBlocked);

        /// Method sending switch command for another window. It will switch window within active application.
        /// to switch windows between applications use sapm::ApplicationManager::messageSwitchApplication
        /// it will effectivelly trigger setActiveWindow and change on windows stack
        int switchWindow(const std::string &windowName,
                         gui::ShowMode cmd                     = gui::ShowMode::GUI_SHOW_INIT,
                         std::unique_ptr<gui::SwitchData> data = nullptr);

        /// Same as switchWindow above
        inline auto switchWindow(const std::string &windowName, std::unique_ptr<gui::SwitchData> data)
        {
            return switchWindow(windowName, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        };

        bool returnToPreviousWindow();

        /// Method refreshing active window
        int refreshWindow(gui::RefreshModes mode);

        /// Mehtod to handle bus messages, all message types are defined in Message.hpp
        /// Message types are in MessageType
        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl);

        /// initialization function
        /// 1. it has to be called for each and every application instance, it registeres application in
        /// ApplicationManager
        ///    and notify ApplicationManager that it's in Initialization state
        /// 2. It gets settings from database
        sys::ReturnCodes InitHandler() override;

        /// function to set active window for application
        /// if none window is selected main window is used
        /// if window name is "LastWindow" then previous window is selected
        /// it modifies windows stack
        void setActiveWindow(const std::string &windowName);

        /// getter for settings record
        SettingsRecord &getSettings()
        {
            return settings;
        }

        /// see suspendInProgress documentation
        virtual void setSuspendFlag(bool val)
        {
            suspendInProgress = val;
        };
        /// see shutdownInProgress documentation
        virtual void setShutdownFlag()
        {
            shutdownInProgress = true;
        };

        bool adjustCurrentVolume(const audio::Volume step);
        bool increaseCurrentVolume(const audio::Volume step = audio::defaultVolumeStep)
        {
            return adjustCurrentVolume(step);
        }
        bool decreaseCurrentVolume(const audio::Volume step = audio::defaultVolumeStep)
        {
            return adjustCurrentVolume(-step);
        }

        /// @defgroup Application Application static functions
        /// All this functions are meant to be used in ApplicationManager only
        /// @note consider moving these as private elements of ApplicationManager i.e. under names
        /// message::switchApplication etc.
        /// @{
        static bool messageSwitchApplication(sys::Service *sender,
                                             std::string application,
                                             std::string window,
                                             std::unique_ptr<gui::SwitchData> data);
        static bool messageRefreshApplication(sys::Service *sender,
                                              std::string application,
                                              std::string window,
                                              gui::SwitchData *data = nullptr);
        static bool messageCloseApplication(sys::Service *sender, std::string application);
        static bool messageRebuildApplication(sys::Service *sender, std::string application);
        /// @}

      protected:
        /// application's settings taken from database
        SettingsRecord settings;

        void longPressTimerCallback();
        /// function executing functions registered by CreateAppTimer
        /// @param id - timer identification number
        virtual void TickHandler(uint32_t id) override final;
        /// Method used to register all windows and widgets in application
        virtual void createUserInterface() = 0;
        /// Method closing application's windows.
        virtual void destroyUserInterface() = 0;
        /// Map containing application's windows
        /// Right now all application windows are being created on application start in createUserInterface
        /// then all windows are removed at the end of application
        /// @note right now there is no mechanism to postphone window creation
        std::map<std::string, gui::AppWindow *> windows;

        /// stack of visited windows in application
        /// @ingrup AppWindowStack
        std::vector<std::string> windowStack;

      public:
        /// @ingrup AppWindowStack
        /// get to the first time we entered this &window
        bool popToWindow(const std::string &window);
        /// push window to the top of windows stack
        /// @ingrup AppWindowStack
        void pushWindow(const std::string &newWindow);
        /// getter for previous window name
        /// @ingrup AppWindowStack
        const std::string getPrevWindow() const;
        /// clears windows stack
        /// @ingrup AppWindowStack
        void cleanPrevWindw();
        /// getter to get window by name
        /// @note could be possibly used to implement building window on request
        /// @ingrup AppWindowStack
        gui::AppWindow *getWindow(const std::string &window);
        /// getter for current wisible window in application
        /// @ingrup AppWindowStack
        gui::AppWindow *getCurrentWindow();

      protected:
        /// Flag defines whether keyboard input should be processed
        bool acceptInput = false;

        /// key translator to handle
        /// 1. long press transformation (right now we support long press only via top application keyTralator handling)
        /// 2. simple translation of keys 1 to 1 with keyboard
        std::unique_ptr<gui::KeyInputSimpleTranslation> keyTranslator;
        /// Flag defines how application will behave after registration. It can go forground or background
        bool startBackground = false;
        /// Flag which defines whether application initialized suspend mode, this will influence how render message will
        /// sent to gui service. If suspend is true, application manager will receive information from both eink and gui
        /// services if last rendering mesage will be processed.
        bool suspendInProgress = false;
        /// Flag defines case when display needs to be refreshed before closing the system. If flag is set to true next
        /// set of rendering commands will carry information to GUI service that system needs to be closed. After
        /// displaying the screen GUI will notify application manager to request system shutdown.
        bool shutdownInProgress = false;

        /// informs self that there was key press
        /// used to provide a way for long press/multipress handling in application
        static bool messageInputEventApplication(sys::Service *sender,
                                                 std::string application,
                                                 const gui::InputEvent &event);
    };

    /// used in ApplicationManager to start applications
    class ApplicationLauncher
    {
      protected:
        /// name of the application to run
        std::string name = "";
        /// name of the application's owner
        std::string parent = "";
        /// defines whether application can be closed when it looses focus
        bool closeable = true;
        /// defines whether application should be run without gaining focus, it will remian in the BACKGROUND state
        bool startBackground = false;
        /// flag defines whether this application can prevent power manager from changing
        bool preventBlocking = false;

      public:
        ApplicationLauncher(std::string name, bool isCloseable, bool preventBlocking = false)
            : name{name}, closeable{isCloseable}, preventBlocking{preventBlocking} {};
        virtual ~ApplicationLauncher(){};
        std::string getName()
        {
            return name;
        };
        bool isCloseable()
        {
            return closeable;
        };
        bool isBlocking()
        {
            return preventBlocking;
        };

        // virtual method to run the application
        virtual bool run(sys::Service *caller = nullptr)
        {
            return true;
        };
        virtual bool runBackground(sys::Service *caller = nullptr)
        {
            return true;
        };
        std::shared_ptr<Application> handle = nullptr;
    };

    /// application launcher boilerplate
    template <class T> class ApplicationLauncherT : public ApplicationLauncher
    {
      public:
        ApplicationLauncherT(std::string name, bool isCloseable = true) : ApplicationLauncher(name, isCloseable)
        {}
        virtual bool run(sys::Service *caller) override
        {
            parent = (caller == nullptr ? "" : caller->GetName());
            handle = std::make_shared<T>(name, parent);
            return sys::SystemManager::CreateService(handle, caller);
        };
        bool runBackground(sys::Service *caller) override
        {
            parent = (caller == nullptr ? "" : caller->GetName());
            handle = std::make_shared<T>(name, parent, true);
            return sys::SystemManager::CreateService(handle, caller);
        };
    };

    /// creates application launcher per class provided
    template <class T>
    std::unique_ptr<ApplicationLauncherT<T>> CreateLauncher(std::string name, bool isCloseable = true)
    {
        return std::move(std::unique_ptr<ApplicationLauncherT<T>>(new ApplicationLauncherT<T>(name, isCloseable)));
    }

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_HPP_ */
