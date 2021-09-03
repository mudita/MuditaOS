// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Application.hpp>

namespace app
{
    class ApplicationBell : public Application
    {
      public:
        using Application::Application;

      protected:
        void attachPopups(const std::vector<gui::popup::ID> &popupsList) override;
    };
} // namespace app
