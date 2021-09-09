// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <locks/data/LockData.hpp>
#include <locks/widgets/Lock.hpp>
#include <locks/widgets/LockBox.hpp>
#include <locks/windows/LockInputWindow.hpp>

namespace gui
{
    class PhoneLockInputWindow : public LockInputWindow
    {
        locks::PhoneLockInputTypeAction phoneLockInputTypeAction = locks::PhoneLockInputTypeAction::Unlock;

      public:
        PhoneLockInputWindow(app::ApplicationCommon *app, const std::string &window_name);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;
    };

} /* namespace gui */
