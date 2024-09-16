// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <InputEvent.hpp>

namespace locks::PhoneLockedKeysWhitelist
{
    [[nodiscard]] bool isOnWhitelist(const gui::InputEvent &inputEvent);
} // namespace locks::PhoneLockedKeysWhitelist
