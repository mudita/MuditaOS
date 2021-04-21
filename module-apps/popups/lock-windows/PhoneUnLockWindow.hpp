// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "LockWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "popups/lock-widgets/Lock.hpp"
#include "popups/lock-widgets/Lock.hpp"
#include "popups/lock-widgets/LockBox.hpp"
#include "PinLockBaseWindow.hpp"

namespace gui
{
    class PhoneUnlockWindow : public PinLockBaseWindow
    {
        std::unique_ptr<LockBox> lockBox                  = nullptr;

        // method hides or show widgets and sets bars according to provided state
        void setVisibleState();
        auto usesNumericKeys() const noexcept -> bool;

      public:
        PhoneUnlockWindow(app::Application *app, const std::string &window_name);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;
    };

} /* namespace gui */
