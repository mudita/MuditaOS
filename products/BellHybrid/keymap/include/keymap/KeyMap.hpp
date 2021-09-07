// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/input/InputEvent.hpp>

/// Key mapping structure to ease translation between PureOS key definitions and nomenclature used throughout the
/// GUI design
enum class KeyMap
{
    Back          = static_cast<int>(gui::KeyCode::KEY_RF),
    LightPress    = static_cast<int>(gui::KeyCode::KEY_ENTER),
    RotateLeft    = static_cast<int>(gui::KeyCode::KEY_DOWN),
    RotateRight   = static_cast<int>(gui::KeyCode::KEY_UP),
    DeepPressUp   = static_cast<int>(gui::KeyCode::KEY_LEFT),
    DeepPressDown = static_cast<int>(gui::KeyCode::KEY_RIGHT),
};

inline static KeyMap mapKey(gui::KeyCode key)
{
    return KeyMap{key};
}
