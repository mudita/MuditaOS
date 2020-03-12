#pragma once

#include "InputEvent.hpp"
#include "Profile.hpp"
#include "bsp/keyboard/key_codes.hpp"
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <common_data/RawKey.hpp>

namespace gui
{

    // TODO if neccessary `custom` keymap can be added her
    enum class Keymaps
    {
        ABC,
        abc,
        digit,
    };

    class KeyBaseTranslation
    {
      public:
        // previous pressed key (only for pressed keys), used for shortpress and longpress
        RawKey prev_key_press = {};
        // was previous key released? used for longpress only
        bool prev_key_released = true;
        // did previous key already timed out (and send longpress as a result)
        bool prev_key_timedout = false;
        InputEvent set(RawKey key);
        /// timeout keypress (only press) - returns true on timeout'ed keypress
        bool timeout(uint32_t time);
    };

    /// KeyPress translator
    /// simplest 1:1 keys handling, used when application needs simplest key parsing possible
    /// no keys behaviour analysis done here - just mapping
    class KeyInputSimpleTranslation : public KeyBaseTranslation
    {
      public:
        /// translate incomming key
        InputEvent translate(RawKey key);
        /// translate timeout - simulate key release
        InputEvent translate(uint32_t timeout);
    };

    /// translator using & switching KeyMaps for use per widget basis ,called for selected widget, per widget basis
    class KeyInputMappedTranslation : public KeyBaseTranslation
    {
        uint32_t times = 0;

      public:
        bool setProfile(std::string profileName);
        uint32_t handle(RawKey key, const std::string &keymap);
        uint32_t getTimes()
        {
            return times;
        }
    };

} /* namespace gui */
