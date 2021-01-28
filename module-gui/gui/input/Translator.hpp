// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

    /// profiles cache - load once for all
    class Profiles
    {
      private:
        std::map<std::string, gui::Profile> profilesList = {};

        void loadProfile(const std::string &filepath);
        std::vector<std::string> getProfilesNames();
        void init();
        Profile empty;

        static Profiles &get();

      public:
        std::vector<std::string> getAvailableInputLanguages();
        Profile &get(const std::string &name);
    };

    /// translator using & switching KeyMaps for use per widget basis ,called for selected widget, per widget basis
    class KeyInputMappedTranslation : public KeyBaseTranslation
    {
        uint32_t times = 0;
        Profiles profiles;

      public:
        uint32_t handle(RawKey key, const std::string &keymap);
        uint32_t getTimes() const noexcept;
    };

} /* namespace gui */
