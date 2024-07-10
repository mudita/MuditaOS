// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>

namespace gui
{
    class ChangeHomescreenLayoutParams : public app::manager::actions::ActionParams
    {
      public:
        explicit ChangeHomescreenLayoutParams(std::string newHomescreenLayout)
            : app::manager::actions::ActionParams{}, newHomescreenLayout{std::move(newHomescreenLayout)} {};

        std::string getNewHomescreenLayoutName()
        {
            return newHomescreenLayout;
        }

      private:
        std::string newHomescreenLayout{};
    };
} // namespace gui
