// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/StatusBar.hpp>
#include <gui/widgets/header/Header.hpp>
#include <gui/widgets/NavBar.hpp>
#include <gui/widgets/Window.hpp>
#include <Service/Service.hpp>
#include <Service/Message.hpp>
#include "AppWindowConstants.hpp"

namespace app
{
    class ApplicationCommon;
};

namespace gui
{
    /*
     * @brief This is wrapper for gui window used within applications.
     */
    class AppWindow : public Window
    {
      protected:
        /**
         * Information bar for signal, battery and lock icon on the top of the screen.
         */
        gui::status_bar::StatusBar *statusBar = nullptr;
        /**
         * Information bar for window title and additional action buttons.
         */
        gui::header::Header *header = nullptr;
        /**
         * Information bar for the buttons on the bottom of the page.
         */
        gui::nav_bar::NavBar *navBar = nullptr;
        /**
         * Pointer to the application object that owns the window.
         */
        app::ApplicationCommon *application = nullptr;

        /**
         * A function that applies configuration changes to the current status bar configuration.
         */
        using StatusBarConfigurationChangeFunction =
            std::function<status_bar::Configuration(status_bar::Configuration)>;

        /**
         * A flag that is set if current window state requires the phone to stay unlocked
         */
        bool preventsAutoLock      = false;
        bool preventsLongPressLock = false;

      public:
        AppWindow() = delete;
        AppWindow(app::ApplicationCommon *app, std::string name);

        app::ApplicationCommon *getApplication()
        {
            return application;
        };

        std::string getUniqueName() override;

        virtual bool onDatabaseMessage(sys::Message *msg);

        bool updateBluetooth(sys::bluetooth::BluetoothMode mode);
        bool updateAlarmClock(bool status);
        bool updateSim();
        virtual bool updateBatteryStatus();
        bool updateSignalStrength();
        bool updateNetworkAccessTechnology();
        void updatePhoneMode(sys::phone_modes::PhoneMode mode);
        bool updateTethering(const sys::phone_modes::Tethering state);
        [[nodiscard]] bool preventsAutoLocking() const noexcept;
        virtual RefreshModes updateTime();

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void accept(GuiVisitor &visitor) override;

        /**
         * Configure the status bar using window-specific configuration.
         * @param appConfiguration      Application-wide status bar configuration that it to be modified.
         * @return window-specific configuration of the status bar.
         */
        virtual status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration);

        /**
         * Applies configuration change on the current status bar configuration.
         * @param configChange  The function that contains the status bar configuration changes.
         */
        void applyToStatusBar(StatusBarConfigurationChangeFunction configChange);

        void setTitle(const UTF8 &text);
        [[nodiscard]] UTF8 getTitle();

        /// Setting bottom bar temporary text
        /// @param text - navBar text
        /// @param overwriteOthers - set or not other navBar texts to "" (default true)
        void navBarTemporaryMode(const UTF8 &text, bool emptyOthers = true);
        void navBarTemporaryMode(const UTF8 &text, nav_bar::Side side, bool emptyOthers = true);
        void navBarRestoreFromTemporaryMode();
        void setNavBarText(const UTF8 &text, nav_bar::Side side);
        void setNavBarActive(nav_bar::Side side, bool value);
        void clearNavBarText(nav_bar::Side side);

        bool selectSpecialCharacter();

        /// get BoundingBox size of Window "body" area
        /// @note it would be much better to just have "body item" instead
        /// but it would mean not insignificant refactor
        BoundingBox bodySize();
    };

} /* namespace gui */
