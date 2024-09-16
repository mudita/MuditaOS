// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/popups/presenter/PowerOffPresenter.hpp>

namespace gui
{
    class BellFactoryResetPresenter : public gui::AbstractPowerOffPresenter
    {
      public:
        explicit BellFactoryResetPresenter(app::ApplicationCommon *app);

        void powerOff(sys::CloseReason reason) override;
        void reboot() override;

      private:
        app::ApplicationCommon *application;
    };
} // namespace gui
