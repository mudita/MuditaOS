#pragma once

#include <gui/widgets/TopBar.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/Window.hpp>
#include <Service/Service.hpp>
#include <Service/Message.hpp>
#include <service-audio/api/AudioServiceAPI.hpp>

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
            const inline std::string main_window = "MainWindow";
            const inline std::string no_window   = "";
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
        AppWindow(app::Application *app, std::string name, uint32_t id = GUIWindowID++);
        AppWindow(AppWindow *win);
        virtual ~AppWindow();

        app::Application *getApplication()
        {
            return application;
        };
        void setApplication(app::Application *app)
        {
            application = app;
        };
        virtual bool onDatabaseMessage(sys::Message *msg);

        bool batteryCharging(bool charging);
        bool setSIM();
        // updates battery level in the window
        bool updateBatteryLevel(uint32_t percentage);
        // updates battery level in the window
        bool updateSignalStrength();
        virtual bool updateTime(const UTF8 &timeStr);
        virtual bool updateTime(const uint32_t &timestamp, bool mode24H);
        void setTitle(const UTF8 &text);

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        std::list<DrawCommand *> buildDrawList() override;
        void textModeShowCB(const UTF8 &text);
        bool textSelectSpecialCB();
        bool returnToPreviousView();
        // TODO: possibly move to differetn class
        bool incCurrentVolume(const audio::Volume step = audio::defaultVolumeStep);
        bool decCurrentVolume(const audio::Volume step = audio::defaultVolumeStep);
    };

} /* namespace gui */
