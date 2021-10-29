// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace app
{
    class ApplicationCommon;
}

namespace gui
{
    class AbstractPowerOffPresenter
    {
      public:
        virtual ~AbstractPowerOffPresenter() = default;
        virtual void powerOff()              = 0;
        virtual void factoryReset()          = 0;
        virtual void reboot()                = 0;
    };

    class PowerOffPresenter : public AbstractPowerOffPresenter
    {
      public:
        PowerOffPresenter(app::ApplicationCommon *app);
        void powerOff() override;
        void factoryReset() override;
        void reboot() override;

      private:
        app::ApplicationCommon *application;
    };

} // namespace gui
