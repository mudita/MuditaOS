// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "widgets/PinLockBaseWindow.hpp"
#include "application-settings-new/widgets/ScreenLockBoxSettings.hpp"
#include "application-settings-new/widgets/ScreenLockHandler.hpp"

namespace gui
{
    class ChangePasscodeWindow : public PinLockBaseWindow
    {
      public:
        explicit ChangePasscodeWindow(app::Application *app);
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        void buildBottomBar() override;
        void buildTopBar() override;
        void buildTitleBar() override;
        void buildInterface() override;
        void destroyInterface() override;
        void invalidate() noexcept;
        void setVisibleState();
        void processPasscode();

        ScreenLockHandler screenLockHandler;
        PinLock::LockState lockState{PinLock::LockState::PasscodeRequired};
        std::unique_ptr<ScreenLockBoxSettings> lockBox = nullptr;
    };

} /* namespace gui */
