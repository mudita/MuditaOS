// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "PhoneLockedKeysWhitelist.hpp"

namespace locks::PhoneLockedKeysWhitelist
{
    namespace
    {
        using namespace gui;
        constexpr std::array keysWhitelist = {KeyCode::KEY_TORCH,
                                              KeyCode::KEY_VOLDN,
                                              KeyCode::HEADSET_VOLDN,
                                              KeyCode::KEY_VOLUP,
                                              KeyCode::HEADSET_VOLUP,
                                              KeyCode::SWITCH_DN,
                                              KeyCode::SWITCH_MID,
                                              KeyCode::SWITCH_UP};
    } // anonymous namespace

    [[nodiscard]] bool isOnWhitelist(const gui::InputEvent &inputEvent)
    {
        for (const auto &key : keysWhitelist) {
            if (inputEvent.is(key)) {
                return true;
            }
        }
        return false;
    }
} // namespace locks::PhoneLockedKeysWhitelist
