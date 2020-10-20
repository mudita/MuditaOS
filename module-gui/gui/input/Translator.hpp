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
#include <vfs.hpp>

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

    /// profiles cache - load once for all
    class Profiles
    {
      private:
        const char *profilesFolder                   = "assets/profiles";
        std::map<std::string, gui::Profile> profiles = {};

        void loadProfile(const std::string &filepath)
        {
            LOG_INFO("Load profile: %s", filepath.c_str());
            auto p = Profile(filepath);
            if (p.getName() != std::string()) {
                profiles.insert({p.getName(), std::move(p)});
            }
        }

        std::vector<std::string> getProfilesList(std::string ext)
        {
            std::vector<std::string> profileFiles;
            LOG_INFO("Scanning %s profiles folder: %s", ext.c_str(), profilesFolder);
            auto dirList = vfs.listdir(profilesFolder, ext);

            for (vfs::DirectoryEntry ent : dirList) {
                if (ent.attributes != vfs::FileAttributes::Directory) {
                    profileFiles.push_back(std::string(profilesFolder) + "/" + ent.fileName);
                }
            }

            LOG_INFO("Total number of profiles: %u", static_cast<unsigned int>(profileFiles.size()));
            return profileFiles;
        }

        void init()
        {
            std::vector<std::string> profileFiles = getProfilesList(".kprof");
            for (std::string mapName : profileFiles) {
                if (std::size(mapName)) {
                    loadProfile(mapName);
                }
            }
            if (std::size(profiles) == 0) {
                LOG_ERROR("No keyboard profiles loaded");
            }
        }
        Profile empty;

      public:
        static Profiles &get()
        {
            static Profiles *p;
            if (p == nullptr) {
                p = new Profiles();
                p->init();
            }
            return *p;
        }

        static Profile &get(const std::string &name)
        {
            // if profile not in profile map -> load
            if (std::size(name) == 0) {
                LOG_ERROR("Request for non existend profile: %s", name.c_str());
                return get().empty;
            }
            if (get().profiles.find(name) == get().profiles.end()) {
                get().loadProfile(name);
            }
            return get().profiles[name];
        }
    };

} /* namespace gui */
