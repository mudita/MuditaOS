// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Translator.hpp"
#include <log.hpp>
#include <algorithm>
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
            evt.setState(InputEvent::State::keyPressed);
            break;
        case RawKey::State::Released:
            translateRelease(evt, key);
            break;
        case RawKey::State::Moved:
            evt.setState(InputEvent::State::keyMoved);
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
            evt.setState(InputEvent::State::keyReleasedLong);
        }
        else {
            evt.setState(InputEvent::State::keyReleasedShort);
        }
    }

    bool KeyBaseTranslation::isKeyPressTimedOut(uint32_t actualTimeStamp)
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
            break;
        case bsp::KeyCodes::NumericKey2:
            return gui::KeyCode::KEY_2;
            break;
        case bsp::KeyCodes::NumericKey3:
            return gui::KeyCode::KEY_3;
            break;
        case bsp::KeyCodes::NumericKey4:
            return gui::KeyCode::KEY_4;
            break;
        case bsp::KeyCodes::NumericKey5:
            return gui::KeyCode::KEY_5;
            break;
        case bsp::KeyCodes::NumericKey6:
            return gui::KeyCode::KEY_6;
            break;
        case bsp::KeyCodes::NumericKey7:
            return gui::KeyCode::KEY_7;
            break;
        case bsp::KeyCodes::NumericKey8:
            return gui::KeyCode::KEY_8;
            break;
        case bsp::KeyCodes::NumericKey9:
            return gui::KeyCode::KEY_9;
            break;
        case bsp::KeyCodes::NumericKey0:
            return gui::KeyCode::KEY_0;
            break;
        case bsp::KeyCodes::NumericKeyAst:
            return gui::KeyCode::KEY_AST;
            break;
        case bsp::KeyCodes::NumericKeyPnd:
            return gui::KeyCode::KEY_PND;
            break;
        case bsp::KeyCodes::JoystickLeft:
            return gui::KeyCode::KEY_LEFT;
            break;
        case bsp::KeyCodes::JoystickRight:
            return gui::KeyCode::KEY_RIGHT;
            break;
        case bsp::KeyCodes::JoystickUp:
            return gui::KeyCode::KEY_UP;
            break;
        case bsp::KeyCodes::JoystickDown:
            return gui::KeyCode::KEY_DOWN;
            break;
        case bsp::KeyCodes::JoystickEnter:
            return gui::KeyCode::KEY_ENTER;
            break;
        case bsp::KeyCodes::FnLeft:
            return gui::KeyCode::KEY_LF;
            break;
        case bsp::KeyCodes::FnRight:
            return gui::KeyCode::KEY_RF;
            break;
        case bsp::KeyCodes::VolUp:
            return gui::KeyCode::KEY_VOLUP;
            break;
        case bsp::KeyCodes::VolDown:
            return gui::KeyCode::KEY_VOLDN;
            break;
        case bsp::KeyCodes::Torch:
            return gui::KeyCode::KEY_TORCH;
            break;
        case bsp::KeyCodes::SSwitchUp:
            return gui::KeyCode::SWITCH_UP;
            break;
        case bsp::KeyCodes::SSwitchDown:
            return gui::KeyCode::SWITCH_DN;
            break;
        case bsp::KeyCodes::SSwitchMid:
            return gui::KeyCode::SWITCH_MID;
            break;
        case bsp::KeyCodes::HeadsetOk:
            return gui::KeyCode::HEADSET_OK;

        case bsp::KeyCodes::HeadsetVolUp:
            void resetPreviousKeyPress();
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
        // when last action timed out we don't want to handle key release
        if (isPreviousKeyTimedOut && key.state == RawKey::State::Released) {
            evt.setState(InputEvent::State::Undefined);
            isPreviousKeyTimedOut = false;
        }
        evt.setKeyCode(getKeyCode(key.keyCode));
        return evt;
    }

    InputEvent KeyInputSimpleTranslation::translate(uint32_t timeout)
    {
        RawKey key{RawKey::State::Released, previousKeyPress.keyCode, 0, timeout};
        return InputEvent{key, InputEvent::State::keyReleasedLong, getKeyCode(key.keyCode)};
    }

    uint32_t KeyInputMappedTranslation::handle(RawKey key, const std::string &keymap)
    {
        // get shortpress
        if (previousKeyPress.keyCode != key.keyCode) {
            times = 0;
        }
        else if (key.state == RawKey::State::Released) {
            if (key.timeRelease - previousKeyPress.timeRelease < keyTimeCycleMs) {
                ++times;
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

    uint32_t KeyInputMappedTranslation::getTimes() const noexcept
    {
        return times;
    }

    void Profiles::loadProfile(const std::string &filepath)
    {
        LOG_INFO("Load profile: %s", filepath.c_str());
        auto p = Profile(filepath);
        if (auto name = p.getName(); !name.empty()) {
            profilesList.insert({p.getName(), std::move(p)});
        }
    }

    std::vector<std::string> Profiles::getProfilesNames()
    {
        std::vector<std::string> profilesNames;
        LOG_INFO("Scanning %s profiles folder: %s", utils::files::jsonExtension, utils::getInputLanguagePath().c_str());

        for (const auto &entry : std::filesystem::directory_iterator(utils::getInputLanguagePath())) {
            profilesNames.push_back(std::filesystem::path(entry.path().stem()));
        }

        LOG_INFO("Total number of profiles: %u", static_cast<unsigned int>(profilesNames.size()));
        return profilesNames;
    }

    std::vector<std::string> Profiles::getAvailableInputLanguages()
    {
        std::vector<std::string> profilesNames = getProfilesNames(), availableProfiles;

        for (auto &name : profilesNames) {
            auto profile = get().profilesList[name];
            if (profile.getJson()[filetype::jsonKey] == filetype::normal) {
                auto breakSignPosition            = name.find_last_of(utils::files::breakSign);
                std::string displayedLanguageName = name.substr(0, breakSignPosition);

                if (std::find(availableProfiles.begin(), availableProfiles.end(), displayedLanguageName) ==
                    availableProfiles.end()) {
                    availableProfiles.push_back(displayedLanguageName);
                }
            }
        }
        return availableProfiles;
    }

    void Profiles::init()
    {
        std::vector<std::string> profilesNames = getProfilesNames();
        for (const auto &profileName : profilesNames) {
            if (!profileName.empty()) {
                auto filePath = utils::getInputLanguagePath() / (profileName + utils::files::jsonExtension);
                loadProfile(filePath);
            }
        }
        if (std::size(profilesList) == 0) {
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
        std::filesystem::path filepath = utils::getInputLanguagePath() / (name + utils::files::jsonExtension);
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

} /* namespace gui */
