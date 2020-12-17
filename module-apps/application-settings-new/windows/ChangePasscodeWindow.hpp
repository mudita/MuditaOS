// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "application-desktop/windows/PinLockBaseWindow.hpp"
#include "application-desktop/windows/PinLockBox.hpp"
#include "application-desktop/widgets/PinLockHandler.hpp"

namespace gui
{
    class ChangePasscodeWindow : public PinLockBaseWindow
    {
      public:
        explicit ChangePasscodeWindow(app::Application *app);
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        void buildInterface() override;

        std::unique_ptr<PinLockBox> lockBox = nullptr;
    };

} /* namespace gui */
