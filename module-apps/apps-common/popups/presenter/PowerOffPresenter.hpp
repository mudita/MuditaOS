// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <system/Common.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace gui
{
    class AbstractPowerOffPresenter
    {
      public:
        virtual ~AbstractPowerOffPresenter()                                                = default;
        virtual void powerOff(sys::CloseReason reason = sys::CloseReason::RegularPowerDown) = 0;
        virtual void reboot()                                                               = 0;
    };

    class PowerOffPresenter : public AbstractPowerOffPresenter
    {
      public:
        PowerOffPresenter(app::ApplicationCommon *app);
        void powerOff(sys::CloseReason reason = sys::CloseReason::RegularPowerDown) override;
        void reboot() override;

      private:
        app::ApplicationCommon *application;
    };

} // namespace gui
