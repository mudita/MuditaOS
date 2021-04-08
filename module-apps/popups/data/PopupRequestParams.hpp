// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/Popups.hpp"

#include <service-appmgr/Actions.hpp>
#include <module-sys/PhoneModes/Common.hpp>

namespace gui
{
    class PopupRequestParams : public app::manager::actions::ActionParams
    {
      public:
        explicit PopupRequestParams(gui::popup::ID popupId)
            : app::manager::actions::ActionParams{"Popup request parameters"}, popupId{popupId}
        {}

        [[nodiscard]] auto getPopupId() const noexcept
        {
            return popupId;
        }

      private:
        gui::popup::ID popupId;
    };

    class PhoneModePopupRequestParams : public PopupRequestParams
    {
      public:
        explicit PhoneModePopupRequestParams(sys::phone_modes::PhoneMode mode)
            : PopupRequestParams{gui::popup::ID::PhoneModes}, phoneMode{mode}
        {}

        [[nodiscard]] auto getPhoneMode() const noexcept
        {
            return phoneMode;
        }

      private:
        sys::phone_modes::PhoneMode phoneMode;
    };
} // namespace gui
