// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <ApplicationCommon.hpp>

namespace app
{
    class Application : public ApplicationCommon
    {
      protected:
        void updateStatuses(gui::AppWindow *window) const override;
        void attachPopups(const std::vector<gui::popup::ID> &popupsList) override;

      public:
        using ApplicationCommon::ApplicationCommon;
    };
} // namespace app
