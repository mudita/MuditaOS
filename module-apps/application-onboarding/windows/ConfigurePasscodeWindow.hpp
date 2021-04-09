// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <popups/lock-widgets/ScreenLockBaseBox.hpp>
#include <popups/lock-windows/LockWindow.hpp>
#include <application-settings-new/windows/ChangePasscodeWindow.hpp>
#include <application-settings-new/data/ChangePasscodeData.hpp>
#include <application-settings-new/widgets/ChangePasscodeLockHandler.hpp>

namespace gui
{
    class ConfigurePasscodeWindow : public ChangePasscodeWindow
    {
      public:
        explicit ConfigurePasscodeWindow(app::Application *app);
        bool onInput(const InputEvent &inputEvent) override;

      protected:
        void buildBottomBar() override;
        void buildTitleBar() override;
        void processPasscode() override;
        void setVisibleState() override;
    };
} /* namespace gui */
