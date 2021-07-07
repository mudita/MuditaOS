// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneLockedKeysWhitelist.hpp"
#include <array>

namespace locks::PhoneLockedKeysWhitelist
{
    namespace
    {
        using namespace gui;
        constexpr std::array list = {
            KeyCode::KEY_TORCH, KeyCode::KEY_VOLDN, KeyCode::HEADSET_VOLDN, KeyCode::KEY_VOLUP, KeyCode::HEADSET_VOLUP};
    } // anonymous namespace

    [[nodiscard]] bool isOnWhitelist(const gui::InputEvent &inputEvent)
    {
        for (const auto &key : list) {
            if (inputEvent.is(key)) {
                return true;
            }
        }
        return false;
    }
} // namespace locks::PhoneLockedKeysWhitelist
