// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "InputEvent.hpp"
#include "Profile.hpp"
#include <hal/key_input/RawKey.hpp>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace gui
{
    class KeyBaseTranslation
    {
      public:
        /// RawKey to Input Event translation
        InputEvent set(RawKey key);
        /// Check if keyPress is timed out for particular timestamp
        bool isKeyPressTimedOut(uint32_t actualTimeStamp);
        /// Reset previous key press status
        void resetPreviousKeyPress();
        /// Set previous key press timeout status
        void setPreviousKeyTimedOut(bool status);

      protected:
        RawKey previousKeyPress    = {};
        bool isPreviousKeyPressed  = false;
        bool isPreviousKeyTimedOut = false;

      private:
        void translateRelease(InputEvent &evt, const RawKey &key);
    };

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
        uint32_t handle(RawKey key, const std::string &keymap, bool incrementTimes = true);
        uint32_t getTimes() const noexcept;
    };

} /* namespace gui */
