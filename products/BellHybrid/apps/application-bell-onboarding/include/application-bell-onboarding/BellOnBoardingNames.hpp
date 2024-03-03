// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/BellCommonNames.hpp>

namespace app
{
    inline constexpr auto applicationBellOnBoardingName = "ApplicationBellOnBoarding";
} // namespace app

namespace gui::window::name
{
    inline constexpr auto onBoardingLanguageWindow          = "BellOnBoardingLanguage";
    inline constexpr auto onBoardingShortcutsOptionWindow   = "BellOnBoardingShortcutsOption";
    inline constexpr auto onBoardingShortcutsWindow         = gui::window::name::shortcutsWindow;
    inline constexpr auto onBoardingUpdateInstructionWindow = gui::window::name::updateInstructionWindow;
    inline constexpr auto onBoardingSettingsWindow          = "BellOnBoardingSettings";
    inline constexpr auto finalizeOnBoardingWindow          = "BellOnBoardingFinalize";
    inline constexpr auto informationOnBoardingWindow       = "BellOnBoardingInformation";
} // namespace gui::window::name
