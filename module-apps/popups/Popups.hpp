// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/popups/VolumeWindow.hpp>
#include <module-apps/popups/HomeModesWindow.hpp>

#include <string>

namespace gui
{
    namespace popup
    {
        enum class ID
        {
            Volume,
            PhoneModes,
            Brightness,
            Tethering,
            TetheringPhoneModeChangeProhibited
        };

        namespace window
        {
            inline constexpr auto volume_window      = "VolumePopup";
            inline constexpr auto phone_modes_window = "PhoneModesPopup";
            inline constexpr auto brightness_window  = "BrightnessPopup";
            inline constexpr auto tethering_confirmation_window     = "TetheringConfirmationPopup";
            inline constexpr auto tethering_phonemode_change_window = "TetheringPhoneModeChangeProhibitedPopup";

        } // namespace window

        std::string resolveWindowName(ID id);
    }     // namespace popup
} // namespace gui
