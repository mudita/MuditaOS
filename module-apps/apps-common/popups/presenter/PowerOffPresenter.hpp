// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace app
{
    class ApplicationCommon;
}

namespace gui
{
    class PowerOffPresenter
    {
      public:
        PowerOffPresenter(app::ApplicationCommon *app);
        void powerOff();

      private:
        app::ApplicationCommon *application;
    };

} // namespace gui
