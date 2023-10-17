// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/Popups.hpp"

#include <service-appmgr/Actions.hpp>
#include <PhoneModes/Common.hpp>

namespace gui
{
    class TetheringParams : public app::manager::actions::ActionParams
    {
      public:
        explicit TetheringParams(sys::phone_modes::Tethering state) : tetheringState{state}
        {}

        [[nodiscard]] auto getTetheringState() const noexcept
        {
            return tetheringState;
        }

      private:
        sys::phone_modes::Tethering tetheringState;
    };
} // namespace gui
