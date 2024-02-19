// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Translator.hpp"
#include <log/log.hpp>
#include <algorithm>
#include <set>
#include <filesystem>
#include "i18n/i18n.hpp"

namespace gui
{
    namespace filetype
    {
        constexpr auto jsonKey = "filetype";
        constexpr auto normal  = "normal";
        constexpr auto special = "special";
    } // namespace filetype

    InputEvent KeyBaseTranslation::set(RawKey key)
    {
        gui::InputEvent evt(key);
        switch (key.state) {
        case RawKey::State::Pressed:
            evt.setState(InputEvent::State::KeyPressed);
            break;
        case RawKey::State::Released:
            translateRelease(evt, key);
            break;
        case RawKey::State::Moved:
            evt.setState(InputEvent::State::KeyMoved);
            break;
        case RawKey::State::Undefined:
            evt.setState(InputEvent::State::Undefined);
            break;
        }
        // store last key press/release
        if (key.state == RawKey::State::Pressed) {
            previousKeyPress = key;
        }
        isPreviousKeyPressed = (key.state == RawKey::State::Pressed);

        return evt;
    }

    void KeyBaseTranslation::translateRelease(InputEvent &evt, const RawKey &key)
    {
        if (!isPreviousKeyPressed) {
            // Release can only happen after Press
            evt.setState(InputEvent::State::Undefined);
            return;
        }
        if ((previousKeyPress.keyCode == key.keyCode) &&
            (key.timeRelease - previousKeyPress.timePress >= keyTimeLongpressMs)) {
            evt.setState(InputEvent::State::KeyReleasedLong);
        }
        else {
            evt.setState(InputEvent::State::KeyReleasedShort);
        }
    }

    bool KeyBaseTranslation::isKeyPressTimedOut(std::uint32_t actualTimeStamp)
    {
        if (isPreviousKeyPressed && (previousKeyPress.timePress != 0) &&
            (actualTimeStamp - previousKeyPress.timePress >= keyTimeLongpressMs)) {
            isPreviousKeyTimedOut = true;
            return true;
        }
        return false;
    }

    void KeyBaseTranslation::resetPreviousKeyPress()
    {
        previousKeyPress = {};
        isPreviousKeyPressed = false;
    }

    void KeyBaseTranslation::setPreviousKeyTimedOut(bool status)
    {
        isPreviousKeyTimedOut = status;
    }

    gui::KeyCode getKeyCode(bsp::KeyCodes code)
    {
        switch (code) {
        case bsp::KeyCodes::NumericKey1:
            return gui::KeyCode::KEY_1;
        case bsp::KeyCodes::NumericKey2:
            return gui::KeyCode::KEY_2;
        case bsp::KeyCodes::NumericKey3:
            return gui::KeyCode::KEY_3;
        case bsp::KeyCodes::NumericKey4:
            return gui::KeyCode::KEY_4;
        case bsp::KeyCodes::NumericKey5:
            return gui::KeyCode::KEY_5;
        case bsp::KeyCodes::NumericKey6:
            return gui::KeyCode::KEY_6;
        case bsp::KeyCodes::NumericKey7:
            return gui::KeyCode::KEY_7;
        case bsp::KeyCodes::NumericKey8:
            return gui::KeyCode::KEY_8;
        case bsp::KeyCodes::NumericKey9:
            return gui::KeyCode::KEY_9;
        case bsp::KeyCodes::NumericKey0:
            return gui::KeyCode::KEY_0;
        case bsp::KeyCodes::NumericKeyAst:
            return gui::KeyCode::KEY_AST;
        case bsp::KeyCodes::NumericKeyPnd:
            return gui::KeyCode::KEY_PND;
        case bsp::KeyCodes::JoystickLeft:
            return gui::KeyCode::KEY_LEFT;
        case bsp::KeyCodes::JoystickRight:
            return gui::KeyCode::KEY_RIGHT;
        case bsp::KeyCodes::JoystickUp:
            return gui::KeyCode::KEY_UP;
        case bsp::KeyCodes::JoystickDown:
            return gui::KeyCode::KEY_DOWN;
        case bsp::KeyCodes::JoystickEnter:
            return gui::KeyCode::KEY_ENTER;
        case bsp::KeyCodes::FnLeft:
            return gui::KeyCode::KEY_LF;
        case bsp::KeyCodes::FnRight:
            return gui::KeyCode::KEY_RF;
        case bsp::KeyCodes::VolUp:
            return gui::KeyCode::KEY_VOLUP;
        case bsp::KeyCodes::VolDown:
            return gui::KeyCode::KEY_VOLDN;
        case bsp::KeyCodes::Torch:
            return gui::KeyCode::KEY_TORCH;
        case bsp::KeyCodes::SSwitchUp:
            return gui::KeyCode::SWITCH_UP;
        case bsp::KeyCodes::SSwitchDown:
            return gui::KeyCode::SWITCH_DN;
        case bsp::KeyCodes::SSwitchMid:
            return gui::KeyCode::SWITCH_MID;
        case bsp::KeyCodes::HeadsetOk:
            return gui::KeyCode::HEADSET_OK;
        case bsp::KeyCodes::HeadsetVolUp:
            return gui::KeyCode::HEADSET_VOLUP;
        case bsp::KeyCodes::HeadsetVolDown:
            return gui::KeyCode::HEADSET_VOLDN;
        default:
            LOG_ERROR("Unhandled bsp key!");
            return gui::KeyCode::KEY_UNDEFINED;
        }
    }

    InputEvent KeyInputSimpleTranslation::translate(RawKey key)
    {
        auto evt = KeyBaseTranslation::set(key);
        if (isPreviousKeyTimedOut && key.state == RawKey::State::Released) {
            // when last action timed out we don't want to handle key release
            evt.setState(InputEvent::State::Undefined);
            isPreviousKeyTimedOut = false;
        }
        else if (key.state == RawKey::State::Moved) {
            // translation of single moved event to keyReleasedShort
            evt.setState(InputEvent::State::KeyReleasedShort);
        }
        evt.setKeyCode(getKeyCode(key.keyCode));
        return evt;
    }

    InputEvent KeyInputSimpleTranslation::translate(std::uint32_t timeout)
    {
        RawKey key{RawKey::State::Released, previousKeyPress.keyCode, 0, timeout};
        return InputEvent{key, InputEvent::State::KeyReleasedLong, getKeyCode(key.keyCode)};
    }

    std::uint32_t KeyInputMappedTranslation::handle(RawKey key, const std::string &keymap, bool incrementTimes)
    {
        // get shortpress
        if (previousKeyPress.keyCode != key.keyCode) {
            times = 0;
        }
        else if (key.state == RawKey::State::Released) {
            if (key.timeRelease - previousKeyPress.timeRelease < keyTimeCycleMs) {
                incrementTimes ? ++times : times;
            }
            else {
                times = 0;
            }
        }
        if (key.state == RawKey::State::Released) {
            previousKeyPress = key;
        }
        return profiles.get(keymap).getCharKey(key.keyCode, times);
    }

    std::uint32_t KeyInputMappedTranslation::getTimes() const noexcept
    {
        return times;
    }

    void Profiles::loadProfile(const std::string &filepath)
    {
        LOG_INFO("Load profile: %s", filepath.c_str());
        auto profile = Profile(filepath);
        if (const auto &name = profile.getName(); !name.empty()) {
            profilesList.insert({name, std::move(profile)});
        }
    }

    std::vector<std::string> Profiles::getProfilesFilenames()
    {
        std::vector<std::string> filenames;
        LOG_INFO("Scanning %s profiles folder: %s", utils::files::jsonExtension, utils::getInputLanguagePath().c_str());

        for (const auto &entry : std::filesystem::directory_iterator(utils::getInputLanguagePath())) {
            filenames.push_back(entry.path());
        }

        LOG_INFO("Total number of profiles: %u", static_cast<unsigned>(filenames.size()));
        return filenames;
    }

    std::vector<std::string> Profiles::getAvailableInputLanguages()
    {
        std::set<std::string> uniqueLanguages;
        const auto &profiles = get().profilesList;
        for (const auto &[name, profile] : profiles) {
            if (profile.getJson()[filetype::jsonKey] == filetype::normal) {
                const auto breakSignPosition      = name.find_last_of(utils::files::breakSign);
                const auto &displayedLanguageName = name.substr(0, breakSignPosition);
                uniqueLanguages.insert(displayedLanguageName);
            }
        }

        std::vector<std::string> availableLanguages;
        std::copy(uniqueLanguages.begin(), uniqueLanguages.end(), std::back_inserter(availableLanguages));
        return availableLanguages;
    }

    void Profiles::init()
    {
        const auto &profilesFilenames = getProfilesFilenames();
        for (const auto &filename : profilesFilenames) {
            if (!filename.empty()) {
                loadProfile(utils::getInputLanguagePath() / filename);
            }
        }
        if (profilesList.empty()) {
            LOG_ERROR("No keyboard profiles loaded");
        }
    }

    Profiles &Profiles::get()
    {
        static Profiles *p;
        if (p == nullptr) {
            p = new Profiles();
            p->init();
        }
        return *p;
    }

    Profile &Profiles::get(const std::string &name)
    {
        const auto &filepath = utils::getInputLanguagePath() / (name + utils::files::jsonExtension);
        // if profile not in profile map -> load
        if (filepath.empty()) {
            LOG_ERROR("Request for nonexistent profile: %s", filepath.c_str());
            return get().empty;
        }
        if (get().profilesList.find(name) == get().profilesList.end()) {
            get().loadProfile(filepath);
        }
        return get().profilesList[name];
    }
} // namespace gui
