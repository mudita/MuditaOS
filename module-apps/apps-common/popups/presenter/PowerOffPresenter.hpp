// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Application.hpp>

namespace gui
{
    class PowerOffPresenter
    {
      public:
        PowerOffPresenter(app::Application *app);
        void powerOff();

      private:
        app::Application *application;
    };

} // namespace gui
