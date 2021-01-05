// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/TopBar.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/Window.hpp>
#include <Service/Service.hpp>
#include <Service/Message.hpp>

namespace app
{
    class Application;
};

namespace gui
{
    namespace name
    {
        namespace window
        {
            inline constexpr auto main_window = "MainWindow";
            inline constexpr auto no_window   = "";
        } // namespace window
    }     // namespace name

    /*
     * @brief This is wrapper for gui window used within applications.
     */
    class AppWindow : public Window
    {
      protected:
        /// actual built window title
        gui::Label *title = nullptr;
        /**
         * Information bar for the buttons on the bottom of the page.
         */
        gui::BottomBar *bottomBar = nullptr;
        /**
         * Information bar for signal, battery and lock icon on the top of the screen.
         */
        gui::TopBar *topBar = nullptr;
        /**
         * Pointer to the application object that owns the window.
         */
        app::Application *application = nullptr;

      public:
        AppWindow() = delete;
        AppWindow(app::Application *app, std::string name);

        app::Application *getApplication()
        {
            return application;
        };
        void setApplication(app::Application *app)
        {
            application = app;
        };
        virtual bool onDatabaseMessage(sys::Message *msg);

        bool updateBatteryCharger(bool charging);
        bool setSIM();
        // updates battery level in the window
        bool updateBatteryLevel(uint32_t percentage);
        // updates battery level in the window
        bool updateSignalStrength();
        bool updateNetworkAccessTechnology();
        virtual bool updateTime(const UTF8 &timeStr);
        virtual bool updateTime(const uint32_t &timestamp, bool mode24H);
        void setTitle(const UTF8 &text);

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void accept(GuiVisitor &visitor) override;
        /// Setting bottom bar temporary text
        /// @param text - bottomBar text
        /// @param overwriteOthers - set or not other bottomBar texts to "" (default true)
        void bottomBarTemporaryMode(const UTF8 &text, bool emptyOthers = true);
        void bottomBarTemporaryMode(const UTF8 &text, BottomBar::Side side, bool emptyOthers = true);
        void bottomBarRestoreFromTemporaryMode();
        void setBottomBarText(const UTF8 &text, BottomBar::Side side);
        void clearBottomBarText(BottomBar::Side side);
        bool selectSpecialCharacter();
        void setBottomBarActive(BottomBar::Side side, bool value);

        /// get BoundingBox size of Window "body" area
        /// @note it would be much better to just have "body item" instead
        /// but it would mean not insignificant refactor
        BoundingBox bodySize();
    };

} /* namespace gui */
