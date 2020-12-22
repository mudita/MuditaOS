// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "widgets/PinLockBaseWindow.hpp"
#include "widgets/PinLockBox.hpp"
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
        void destroyInterface() override;
        void invalidate() noexcept;
        void setVisibleState();

        std::unique_ptr<PinLockBox> lockBox = nullptr;
        uint32_t newPasscodeHash;
    };

} /* namespace gui */
