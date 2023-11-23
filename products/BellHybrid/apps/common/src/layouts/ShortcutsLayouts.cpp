// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/ShortcutsLayouts.hpp"
#include "layouts/ShortcutsLayoutClassic.hpp"

namespace gui::factory
{
    std::vector<LayoutGenerator> getLayouts()
    {
        return {{[]() {
                    return new ShortcutsLayoutClassic(
                        "shortcuts_step_rotate", "app_bell_onboarding_shortcuts_step_rotate", false, true);
                }},
                {[]() {
                    return new ShortcutsLayoutClassic("shortcuts_step_light_click",
                                                      "app_bell_onboarding_shortcuts_step_light_click");
                }},
                {[]() {
                    return new ShortcutsLayoutClassic("shortcuts_step_deep_press",
                                                      "app_bell_onboarding_shortcuts_step_deep_press");
                }},
                {[]() {
                    return new ShortcutsLayoutClassic("shortcuts_step_home_and_battery",
                                                      "app_bell_onboarding_shortcuts_step_home");
                }},
                {[]() {
                    return new ShortcutsLayoutClassic("shortcuts_step_home_and_battery",
                                                      "app_bell_onboarding_shortcuts_step_battery");
                }},
                {[]() {
                    return new ShortcutsLayoutClassic("shortcuts_step_turn_off",
                                                      "app_bell_onboarding_shortcuts_step_turn_off");
                }},
                {[]() {
                    return new ShortcutsLayoutClassic("shortcuts_step_restart",
                                                      "app_bell_onboarding_shortcuts_step_restart");
                }},
                {[]() {
                    return new ShortcutsLayoutClassic(
                        "shortcuts_step_bedside_lamp", "app_bell_onboarding_shortcuts_step_bedside_lamp", true, false);
                }}};
    }
} // namespace gui::factory
