// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AsyncTask.hpp"
#include "Audio/AudioCommon.hpp"      // for Volume, Play...
#include "Audio/Profiles/Profile.hpp" // for Profile, Pro...
#include "CallbackStorage.hpp"

#include "system/Common.hpp"   // for ReturnCodes
#include "Service/Message.hpp" // for MessagePointer
#include "Service/Service.hpp" // for Service
#include "Timers/TimerHandle.hpp"
#include "SwitchData.hpp" // for SwitchData
#include <SystemManager/SystemManagerCommon.hpp>
#include <hal/key_input/KeyEventDefinitions.hpp>
#include "gui/Common.hpp" // for ShowMode
#include "projdefs.h"     // for pdMS_TO_TICKS
#include <PhoneModes/Observer.hpp>

#include <service-appmgr/ApplicationManifest.hpp>
#include <list>     // for list
#include <map>      // for allocator, map
#include <memory>   // for make_shared
#include <stdint.h> // for uint32_t
#include <string>   // for string
#include <utility>  // for move, pair
#include <vector>   // for vector
#include <StatusBarManager.hpp>
#include <popups/Popups.hpp>
#include <locks/handlers/PhoneLockSubject.hpp>
#include <locks/handlers/SimLockSubject.hpp>
#include <locks/handlers/LockPolicyHandler.hpp>
#include "WindowsFactory.hpp"
#include "WindowsPopupQueue.hpp"
#include "PopupBlueprintFactory.hpp"
#include "AppSwitchReason.hpp"
#include "AppStartupReason.hpp"

namespace app
{
    class WindowsStack;
    using UiNotificationFilter = std::function<bool(sys::Message *, const std::string &name)>;
} // namespace app

namespace db
{
    class NotificationMessage;
}

namespace gui
{
    class AppWindow;
    class InputEvent;
    class Item;
    class PopupRequestParams;
    class KeyInputSimpleTranslation;
    namespace popup
    {
        class Filter;
    }
} // namespace gui
namespace settings
{
    class Settings;
}

namespace app
{
    class ApplicationCommon;
    class GuiTimer;

    class ActionHandledResponse; // Forward declaration
    using ActionResult = std::shared_ptr<ActionHandledResponse>;
    auto actionHandled() -> ActionResult;
    auto actionNotHandled() -> ActionResult;

    using ApplicationName = std::string;

    enum class StartupStatus
    {
        Success,
        Failure
    };

    struct StartInBackground
    {
        StartInBackground(bool _value) : value{_value}
        {}

        explicit operator bool() const noexcept
        {
            return value;
        }

        bool value;
    };

    /// Type traits pattern used to enforce user-defined types to implement "GetManifest" function.
    template <class T> struct ManifestTraits;

    template <class, class = void> struct HasManifest : std::false_type
    {};

    /// Checks whether T implements "GetManifest" static method.
    /// Provides the member constant "value" that is equal to true if T implements "GetManifest" static method.
    /// Otherwise, "value" is equal to false.
    template <class T>
    struct HasManifest<T, std::void_t<decltype(&ManifestTraits<T>::GetManifest)>>
        : std::is_same<app::manager::ApplicationManifest, decltype(ManifestTraits<T>::GetManifest())>
    {};

    /// Retrieves the manifest of T, if T implements ManifestTraits.
    /// Otherwise, reports an error during compile time.
    template <class T, std::enable_if_t<HasManifest<T>::value, int> = 0>
    auto ManifestOf() -> manager::ApplicationManifest
    {
        return ManifestTraits<T>::GetManifest();
    }

    class StatusIndicators
    {
      public:
        sys::phone_modes::PhoneMode phoneMode       = sys::phone_modes::PhoneMode::Connected;
        sys::bluetooth::BluetoothMode bluetoothMode = sys::bluetooth::BluetoothMode::Disabled;
        bool alarmClockStatus                       = false;
    };

    /// This is template for creating new applications. Main difference between Application and service is that:
    /// 1. Application has access to GUI and Input
    /// 2. Application lifetime is managed with app::manager::ApplicationManager
    class ApplicationCommon : public sys::Service, public AsyncCallbacksDeleter
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
            /// provided launcher. The other possibility is that Application Manager received CLOSING_FINISHED message.
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
            /// Application lost focus but it is still working
            ACTIVE_BACKGROUND,
            /// Application: Close request message has been received but there are still pending tasks to be resolved
            /// before closure. Application informs Application Manager that it can switch application if needed.
            FINALIZING_CLOSE,
            /// Application: Close request message has been received. As a response application must send close request
            /// acknowledge message. Application must start closing all workers and releasing resources. After all
            /// workers are closed and resources released application sends to application manager CLOSING_FINISHED
            /// message Application Manager sent signal to application that it should be closed. Application must
            /// perform all necessary cleanup and request System Manager to close it.
            DEACTIVATING
        };

        /// c_str() function for Application::State
        static const char *stateStr(State st);

        using OnActionReceived = std::function<ActionResult(manager::actions::ActionParamsPtr &&)>;

      protected:
        virtual sys::MessagePointer handleKBDKeyEvent(sys::Message *msgl);
        virtual sys::MessagePointer handleApplicationSwitch(sys::Message *msgl);
        virtual sys::MessagePointer handleAppClose(sys::Message *msgl);
        virtual sys::MessagePointer handleSwitchWindow(sys::Message *msgl);
        virtual sys::MessagePointer handleAppFocusLost(sys::Message *msgl);
        virtual void updateStatuses(gui::AppWindow *window) const;
        void setDefaultWindow(const std::string &w);

      private:
        std::unique_ptr<gui::popup::Filter> popupFilter;
        std::unique_ptr<WindowsStack> windowsStackImpl;
        WindowsStack &windowsStack() const;
        std::string default_window;
        State state = State::DEACTIVATED;

        sys::MessagePointer handleSignalStrengthUpdate(sys::Message *msgl);
        sys::MessagePointer handleNetworkAccessTechnologyUpdate(sys::Message *msgl);
        sys::MessagePointer handleInputEvent(sys::Message *msgl);
        sys::MessagePointer handleBatteryStatusChange();
        sys::MessagePointer handleMinuteUpdated(sys::Message *msgl);
        sys::MessagePointer handleAction(sys::Message *msgl);
        sys::MessagePointer handleApplicationSwitchLaunch(sys::Message *msgl);
        sys::MessagePointer handleApplicationSwitchOnAction(sys::Message *msgl);
        /// this method filters out:
        /// 1. refresh for windows we do not have builder
        /// 2. refresh for windows which are not on top (to avoid building things we do not want to show
        ///    i.e. due to delayed refresh
        sys::MessagePointer handleUpdateWindow(sys::Message *msgl);
        bool handleUpdateTextRefresh(gui::SwitchData *data);
        sys::MessagePointer handleAppRebuild(sys::Message *msgl);
        sys::MessagePointer handleAppRefresh(sys::Message *msgl);
        sys::MessagePointer handleGetDOM(sys::Message *msgl);
        sys::MessagePointer handleSimStateUpdateMessage(sys::Message *msgl);

        std::list<std::unique_ptr<app::GuiTimer>> gui_timers;
        std::unordered_map<manager::actions::ActionId, OnActionReceived> receivers;
        void switchWindowPopup(const std::string &windowName,
                               const gui::popup::Disposition &disposition,
                               std::unique_ptr<gui::SwitchData> data = nullptr,
                               SwitchReason reason                   = SwitchReason::SwitchRequest);

      public:
        sys::TimerHandle longPressTimer;
        void clearLongPressTimeout();

        explicit ApplicationCommon(std::string name,
                                   std::string parent                  = "",
                                   StatusIndicators statusIndicators   = StatusIndicators{},
                                   StartInBackground startInBackground = {false},
                                   uint32_t stackDepth                 = 4096,
                                   sys::ServicePriority priority       = sys::ServicePriority::Idle);

        virtual ~ApplicationCommon() noexcept;

        ApplicationCommon::State getState();
        void setState(State st);

        /// Method responsible for rendering currently active window.
        /// 1. queries for static data for all windows form Store (i.e. battery level, sim card level)
        /// 2. loads rendering commands and send them to GUI renderer (sgui::ServiceGUI)
        void render(gui::RefreshModes mode);

        /// Responsible for sending updated data to the current window.
        /// @param data contextual data for windows flow control.
        /// @param cmd command for Application, right now it's either: Start or Return. As user only Start is used.
        /// @param refreshMode refresh mode to be used when updating window.
        void updateCurrentWindow(std::unique_ptr<gui::SwitchData> data = nullptr,
                                 gui::ShowMode command                 = gui::ShowMode::GUI_SHOW_INIT,
                                 gui::RefreshModes refreshMode         = gui::RefreshModes::GUI_REFRESH_FAST);

        /// Method sending switch command for another window. It will switch window within active application.
        /// To switch windows between applications use app::manager::Controller::switchApplication
        ///
        /// @param windowName name of window to show, only required parameter, our windows stack uses string names as
        /// id's
        /// @param cmd command for Application, right now it's either: Start or Return. As user only Start is used.
        /// @param data contextual data for UI Windows flow steering. Base SwitchData enables user to:
        ///        1. ignoreCurrentWindowOnStack: omit window from windows stack, so that it won't be shown when back (
        ///        RF key) is used
        ///        2. disableAppClose: for use with messageSwitchApplication(...) when switching through app to inform
        ///        that we dont want to close app calling
        void switchWindow(const std::string &windowName,
                          gui::ShowMode cmd                     = gui::ShowMode::GUI_SHOW_INIT,
                          std::unique_ptr<gui::SwitchData> data = nullptr,
                          SwitchReason reason                   = SwitchReason::SwitchRequest);

        /// Same as switchWindow above
        inline void switchWindow(const std::string &windowName, std::unique_ptr<gui::SwitchData> data)
        {
            switchWindow(windowName, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        };

        /// Method used to go back to desired window by using the index difference on stack value
        /// @param ignoredWindowsNumber: defines how many windows will be skipped while going back on stack
        [[deprecated]] void returnToPreviousWindow();

        /// Find and pop window from stack by window name
        void popWindow(const std::string &window);

        void registerOnPopCallback(std::function<void(WindowsStack &)> callback);

        /// Pops the current window from the windows stack
        void popCurrentWindow();

        /// Method refreshing active window
        void refreshWindow(gui::RefreshModes mode);

        /// to not hide overload from parent
        using Service::DataReceivedHandler;

        /// Mehtod to handle bus messages, all message types are defined in Message.hpp
        /// Message types are in MessageType
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl);

        /// initialization function
        /// 1. it has to be called for each and every application instance, it registeres application in
        /// ApplicationManager
        ///    and notify ApplicationManager that it's in Initialization state
        /// 2. It gets settings from database
        sys::ReturnCodes InitHandler() override;

        /// Deinitialization function.
        /// Called upon Application exit and/or termination request.
        sys::ReturnCodes DeinitHandler() override;

        /// see suspendInProgress documentation
        virtual void setSuspendFlag(bool val)
        {
            suspendInProgress = val;
        };

        // Latching close system in progress flag
        virtual void setSystemCloseInProgress()
        {
            systemCloseInProgress = true;
        }

        bool adjustCurrentVolume(const int step);
        bool increaseCurrentVolume(const audio::Volume step = audio::defaultVolumeStep)
        {
            return adjustCurrentVolume(step);
        }
        bool decreaseCurrentVolume(const audio::Volume step = audio::defaultVolumeStep)
        {
            return adjustCurrentVolume(-step);
        }

        void toggleTorchOnOff();

        void toggleTorchColor();

        void cancelCallbacks(AsyncCallbackReceiver::Ptr receiver) override;

        /// @defgroup Application Application static functions
        /// All this functions are meant to be used in ApplicationManager only
        /// @note consider moving these as private elements of ApplicationManager i.e. under names
        /// message::switchApplication etc.
        /// @{
        static void requestAction(sys::Service *sender,
                                  const ApplicationName &applicationName,
                                  manager::actions::ActionId actionId,
                                  manager::actions::ActionParamsPtr &&data);
        static void messageSwitchApplication(sys::Service *sender,
                                             std::string application,
                                             std::string window,
                                             std::unique_ptr<gui::SwitchData> data,
                                             StartupReason startupReason);
        static void messageCloseApplication(sys::Service *sender, std::string application);
        static void messageRebuildApplication(sys::Service *sender, std::string application);
        static void messageApplicationLostFocus(sys::Service *sender, std::string application);
        static void messageSwitchBack(sys::Service *sender, const std::string &application);
        /// @}

      protected:
        void longPressTimerCallback();
        /// Method to register all possible popups to handle in application
        virtual void registerPopupBlueprints();
        /// fallback method  to get popup if none is added
        std::optional<gui::popup::Blueprint> popupBlueprintFallback(gui::popup::ID id);
        /// Method used to register all windows and widgets in application
        virtual void createUserInterface() = 0;
        /// Method closing application's windows.
        virtual void destroyUserInterface() = 0;

        /// @ingrup AppWindowStack
        WindowsFactory windowsFactory;
        std::unique_ptr<WindowsPopupQueue> windowsPopupQueue = std::make_unique<WindowsPopupQueue>();
        PopupBlueprintFactory popupBlueprint;

        /// Method used to attach popups windows to application
        virtual void attachPopups(const std::vector<gui::popup::ID> &popupsList) = 0;
        virtual void actionPopupPush(std::unique_ptr<gui::SwitchData> params);
        virtual bool tryShowPopup();
        void abortPopup(gui::popup::ID id);

        bool userInterfaceDBNotification(sys::Message *msg, const UiNotificationFilter &filter = nullptr);
        virtual gui::popup::Filter &getPopupFilter() const;

      public:
        /// push window to the top of windows stack
        /// @ingrup AppWindowStack
        void pushWindow(const std::string &newWindow, const gui::popup::Disposition &d = gui::popup::WindowDisposition);
        /// getter for previous window name
        /// @ingrup AppWindowStack
        std::optional<std::string> getPrevWindow(uint32_t count = 1) const;
        /// clears windows stack
        /// @ingrup AppWindowStack
        void cleanPrevWindw();
        /// getter for current wisible window in application
        /// if there is none - returns default window
        /// @ingrup AppWindowStack
        gui::AppWindow *getCurrentWindow();
        /// @ingrup AppWindowStack
        bool isCurrentWindow(const std::string &windowName) const noexcept;

        /// @ingrup AppWindowStack
        gui::AppWindow *getWindow(const std::string &name);
        /// to avoid conflicts with connect below
        using Service::connect;
        /// connects item with GuiTimer and stores it in app
        void connect(GuiTimer *timer, gui::Item *item);

      protected:
        /// key translator to handle
        /// 1. long press transformation (right now we support long press only via top application keyTralator handling)
        /// 2. simple translation of keys 1 to 1 with keyboard
        std::unique_ptr<gui::KeyInputSimpleTranslation> keyTranslator;
        /// Flag defines how application will behave after registration. It can go forground or background
        StartInBackground startInBackground{false};
        /// Flag which defines whether application initialized suspend mode, this will influence how render message will
        /// sent to gui service. If suspend is true, application manager will receive information from both eink and gui
        /// services if last rendering mesage will be processed.
        bool suspendInProgress = false;

        bool systemCloseInProgress = false;
        /// Storage for asynchronous tasks callbacks.
        std::unique_ptr<CallbackStorage> callbackStorage;
        void checkBlockingRequests();
        virtual sys::MessagePointer handleAsyncResponse(sys::ResponseMessage *resp);

        friend class AsyncTask; // Async tasks need access to application internals, e.g. callback storage, to make
        // their API simple.

        /// informs self that there was key press
        /// used to provide a way for long press/multipress handling in application
        static void messageInputEventApplication(sys::Service *sender,
                                                 std::string application,
                                                 const gui::InputEvent &event);

        void addActionReceiver(manager::actions::ActionId actionId, OnActionReceived &&callback);
        virtual void handleNotificationsChanged(std::unique_ptr<gui::SwitchData> notificationsParams);
        gui::RefreshModes getRefreshModeFromNotifications(gui::SwitchData *notificationsParams);

        std::unique_ptr<StatusBarManager> statusBarManager;

        /// application's settings
        std::unique_ptr<settings::Settings> settings;
        StatusIndicators statusIndicators;

        locks::PhoneLockSubject phoneLockSubject;
        locks::LockPolicyHandler lockPolicyHandler;
        locks::SimLockSubject simLockSubject;

      public:
        [[nodiscard]] auto getPhoneLockSubject() noexcept -> locks::PhoneLockSubject &;
        [[nodiscard]] auto getSimLockSubject() noexcept -> locks::SimLockSubject &;

        [[nodiscard]] bool isPhoneLockEnabled() const noexcept;

        [[nodiscard]] auto getLockPolicyHandler() noexcept -> locks::LockPolicyHandlerInterface &;

        const gui::status_bar::Configuration &getStatusBarConfiguration() const noexcept;
    };

    /// Parameter pack used by application launch action.
    class ApplicationLaunchData : public manager::actions::ActionParams
    {
      public:
        explicit ApplicationLaunchData(const app::ApplicationName &appName)
            : manager::actions::ActionParams{"Application launch parameters"}, targetAppName{appName}
        {}

        [[nodiscard]] auto getTargetApplicationName() const noexcept
        {
            return targetAppName;
        }

      private:
        ApplicationName targetAppName;
    };
} /* namespace app */
