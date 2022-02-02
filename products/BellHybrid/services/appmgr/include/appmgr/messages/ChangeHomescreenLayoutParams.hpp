// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>

namespace gui
{
    class ChangeHomescreenLayoutParams : public app::manager::actions::ActionParams
    {
      public:
        ChangeHomescreenLayoutParams(std::string newHomescreenLayout)
            : app::manager::actions::ActionParams{}, newHomescreenLayout{newHomescreenLayout} {};

        std::string getNewHomescreenLayoutName()
        {
            return newHomescreenLayout;
        }

      private:
        std::string newHomescreenLayout{};
    };
} // namespace gui
