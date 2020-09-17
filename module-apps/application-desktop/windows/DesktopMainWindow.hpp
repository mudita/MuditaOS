#pragma once

#include "../ApplicationDesktop.hpp"
#include "AppWindow.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Text.hpp"
#include "gui/widgets/TopBar.hpp"
#include "gui/widgets/Window.hpp"

namespace gui
{

    class DesktopMainWindow : public AppWindow
    {
      protected:
        gui::Label *description   = nullptr;
        gui::Label *time          = nullptr;
        gui::Label *dayText       = nullptr;
        gui::Image *messagesImage = nullptr;
        gui::VBox *notifications  = nullptr;

        /// Timed enter value cache, could be templated to any value really
        class EnterCache
        {
            /// to tell if enter was pressed or not
            bool enterPressed = false;
            /// val to check timeout
            uint32_t unlockStartTime = 0;
            /// val to clear start time
            uint32_t unclockTime = 3000;

          public:
            bool storeEnter(const InputEvent &evt)
            {
                enterPressed    = evt.is(KeyCode::KEY_ENTER);
                unlockStartTime = xTaskGetTickCount();
                return enterPressed;
            }

            void clear()
            {
                enterPressed = false;
            }

            bool cached()
            {
                // value timed out -> clear cache
                if (!(xTaskGetTickCount() - unlockStartTime < unclockTime)) {
                    enterPressed = false;
                }
                return enterPressed;
            }
        } enter_cache;
        /**
         * Name of the appliction that was on top when lock timeout occured
         */
        std::string lockTimeoutApplilcation = "";

        // method hides or show widgets and sets bars according to provided state
        void setVisibleState();
        auto fillNotifications(app::ApplicationDesktop *app) -> bool;

      public:
        DesktopMainWindow(app::Application *app);

        // virtual methods gui::Window
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        // virtual methods gui::AppWindow
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool updateTime(const UTF8 &timeStr) override;
        bool updateTime(const uint32_t &timestamp, bool mode24H) override;
        std::list<DrawCommand *> buildDrawList() override;

      private:
        gui::KeyInputMappedTranslation translator;
    };

} /* namespace gui */
