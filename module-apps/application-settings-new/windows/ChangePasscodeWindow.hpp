// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-desktop/windows/LockWindow.hpp"
#include "application-desktop/windows/ScreenLockBaseBox.hpp"
#include "application-settings-new/data/ChangePasscodeData.hpp"
#include "application-settings-new/widgets/ChangePasscodeLockHandler.hpp"

namespace gui
{
    class ChangePasscodeWindow : public LockWindow
    {
      public:
        explicit ChangePasscodeWindow(app::Application *app);
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        void buildBottomBar() override;
        void buildInterface() override;
        void buildTitleBar() override;
        void buildTopBar() override;
        void destroyInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void processPasscode();
        void setVisibleState();

        ChangePasscodeAction changePasscodeAction;
        std::unique_ptr<ScreenLockBaseBox> lockBox = nullptr;
        PinLock::LockState lockState{PinLock::LockState::PasscodeRequired};
        ChangePasscodeLockHandler lockHandler;
    };
} /* namespace gui */
