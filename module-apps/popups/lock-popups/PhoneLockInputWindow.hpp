// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <locks/data/LockData.hpp>
#include <locks/widgets/Lock.hpp>
#include <locks/widgets/LockBox.hpp>
#include <locks/windows/PinLockBaseWindow.hpp>

namespace gui
{
    class PhoneLockInputWindow : public PinLockBaseWindow
    {
        std::unique_ptr<LockBox> lockBox                         = nullptr;
        locks::PhoneLockInputTypeAction phoneLockInputTypeAction = locks::PhoneLockInputTypeAction::Unlock;

        void setVisibleState();
        [[nodiscard]] auto isInInputState() const noexcept -> bool;

      public:
        PhoneLockInputWindow(app::Application *app, const std::string &window_name);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
