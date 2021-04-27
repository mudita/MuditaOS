// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <gui/widgets/Label.hpp>
#include <Translator.hpp>

namespace app
{
    class ApplicationDesktop;
}

namespace gui
{
    class PhoneLockedWindow : public AppWindow
    {
      protected:
        gui::Label *time    = nullptr;
        gui::Label *dayText = nullptr;

        /// Locking timer on pressing enter
        class LockingTimer
        {
            bool enterPressed        = false;
            uint32_t unLockStartTime = 0;
            uint32_t unLockTime      = 3000;

          public:
            bool storeEnter(const InputEvent &evt);
            void clear() noexcept;
            bool cached() noexcept;
        } lockingTimer;

        bool processLongReleaseEvent(const InputEvent &inputEvent);
        bool processShortReleaseEvent(const InputEvent &inputEvent);

      public:
        PhoneLockedWindow(app::Application *app, const std::string &name);

        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void buildInterface() override;
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;

        bool updateTime() override;
    };

} /* namespace gui */
