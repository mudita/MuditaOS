// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "PinLockBox.hpp"
#include "PinLockBaseWindow.hpp"
namespace gui
{
    class PinLockWindow : public PinLockBaseWindow
    {
        const std::string this_window_name;
        std::string lockTimeoutApplication  = "";
        std::unique_ptr<PinLockBox> LockBox = nullptr;
        PinLockBox::EnterPasscodeType currentPasscodeType = PinLockBox::EnterPasscodeType::ProvidePasscode;

        // method hides or show widgets and sets bars according to provided state
        void setVisibleState();
        void buildPinLockBox();
        auto usesNumericKeys() const noexcept -> bool;

      public:
        PinLockWindow(app::Application *app, const std::string &window_name);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
