// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ApplicationCommon.hpp>

namespace app
{
    class Application : public ApplicationCommon
    {
      protected:
        void updateStatuses(gui::AppWindow *window) const override;

      public:
        using ApplicationCommon::ApplicationCommon;
    };
} // namespace app
