// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ApplicationCommon.hpp>

namespace app
{
    class Application : public ApplicationCommon
    {
      public:
        using ApplicationCommon::ApplicationCommon;

      protected:
        void attachPopups(const std::vector<gui::popup::ID> &popupsList) override;
    };
} // namespace app
