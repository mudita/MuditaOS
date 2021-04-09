// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"

#include "Translator.hpp"

namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    class NotificationsBox;

    class PhoneLockedWindow : public AppWindow
    {
      protected:
        gui::Label *time                     = nullptr;
        gui::Label *dayText                  = nullptr;
        gui::NotificationsBox *notifications = nullptr;

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

        bool processLongPressEvent(const InputEvent &inputEvent);
        bool processShortPressEventOnLocked(const InputEvent &inputEvent);

      public:
        PhoneLockedWindow(app::Application *app, const std::string &name);

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void buildInterface() override;
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;

        bool updateTime() override;
    };

} /* namespace gui */
