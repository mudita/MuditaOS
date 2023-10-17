// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/Popups.hpp"

#include <service-appmgr/Actions.hpp>
#include <PhoneModes/Common.hpp>

namespace gui
{
    class PhoneModeParams : public app::manager::actions::ActionParams
    {
      public:
        explicit PhoneModeParams(sys::phone_modes::PhoneMode mode) : phoneMode{mode}
        {}

        [[nodiscard]] auto getPhoneMode() const noexcept
        {
            return phoneMode;
        }

      private:
        sys::phone_modes::PhoneMode phoneMode;
    };
} // namespace gui
