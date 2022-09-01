// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/input/InputEvent.hpp>
#include <bitset>
#include <numeric>

/// Key mapping structure to ease translation between PureOS key definitions and nomenclature used throughout the
/// GUI design
enum class KeyMap : std::uint8_t
{
    Frontlight    = 0,
    Back          = 1,
    LightPress    = 2,
    RotateLeft    = 3,
    RotateRight   = 4,
    DeepPressUp   = 5,
    DeepPressDown = 6
};

inline static KeyMap mapKey(const gui::KeyCode key)
{
    switch (key) {
    case gui::KeyCode::KEY_LF:
        return KeyMap::Frontlight;
    case gui::KeyCode::KEY_RF:
        return KeyMap::Back;
    case gui::KeyCode::KEY_ENTER:
        return KeyMap::LightPress;
    case gui::KeyCode::KEY_DOWN:
        return KeyMap::RotateLeft;
    case gui::KeyCode::KEY_UP:
        return KeyMap::RotateRight;
    case gui::KeyCode::KEY_LEFT:
        return KeyMap::DeepPressUp;
    case gui::KeyCode::KEY_RIGHT:
        return KeyMap::DeepPressDown;
    default:
        assert(("Unrecognized key code"));
        return KeyMap{};
    }
}

inline static KeyMap mapKey(const RawKey &key)
{
    return mapKey(static_cast<gui::KeyCode>(key.keyCode));
}

inline static KeyMap mapKey(const bsp::KeyCodes key)
{
    return mapKey(static_cast<gui::KeyCode>(key));
}

struct KeyStates
{
    using KeySet = std::bitset<32U>;

    void set(KeyMap key, bool value)
    {
        states.set(magic_enum::enum_integer(key), value);
    }
    bool state(KeyMap key) const
    {
        return states.test(magic_enum::enum_integer(key));
    }

    std::size_t count() const
    {
        return states.count();
    }

    template <KeyMap... keys>
    bool ifOnlySet() const
    {
        constexpr auto mask = KeySet{(... | (1UL << magic_enum::enum_integer(keys)))};
        return (states | mask) == mask;
    }

  private:
    KeySet states;
};
