// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"

#include "Translator.hpp"
#include <module-apps/models/NotificationsModel.hpp>
#include <ListView.hpp>

namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    class DesktopMainWindow : public AppWindow
    {
        void onProviderDataChange(bool isOnMainWindow);

      protected:
        gui::Label *time          = nullptr;
        gui::Label *dayText       = nullptr;

        gui::ListView *list                                    = nullptr;
        std::shared_ptr<gui::NotificationsModel> notifications = nullptr;

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
         * Name of the application that was on top when lock timeout occurred
         */
        std::string lockTimeoutApplilcation = "";

        // method hides or show widgets and sets bars according to provided state
        void setVisibleState();
        auto setActiveState() -> bool;
        bool processLongPressEvent(const InputEvent &inputEvent);
        bool processShortPressEventOnUnlocked(const InputEvent &inputEvent);
        bool processShortPressEventOnLocked(const InputEvent &inputEvent);
        app::ApplicationDesktop *getAppDesktop() const;

      public:
        explicit DesktopMainWindow(app::Application *app);

        // virtual methods gui::Window
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        // virtual methods gui::AppWindow
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;

        bool updateTime() override;
        void notificationChanged(app::manager::actions::NotificationsChangedParams *params, bool isOnMainWindow);

      private:
        void invalidate() noexcept;
        std::string osUpdateVer;
        std::string osCurrentVer;

        gui::KeyInputMappedTranslation translator;
    };

} /* namespace gui */
