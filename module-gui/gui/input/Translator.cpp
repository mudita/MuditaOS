#include "Translator.hpp"
#include "log/log.hpp"
#include "vfs.hpp"
#include <algorithm>

namespace gui
{

void KeyInputHandler::keyTimeUpdate()
{
    if (previus.state != RawKey::State::Pressed)
    {
        previus.duration += key_time_base_ms;
    }
}

/// in progress
RawKey KeyInputHandler::handle(uint32_t key_code, RawKey::State state, uint32_t timestamp)
{
    RawKey rawKey{RawKey::State::Undefined, 0, 0, 0};
    if (key_code != previus.key_code)
    {
        /// different key - cleanup old
        previus.reset(key_code, state);
    }

    switch (state)
    {
    case RawKey::State::Released:
        // if we reached longpress timeout -> longpress
        if (previus.duration >= key_time_longpress_ms)
        {
        }
        // if it was fast (500 ms ago - next)
        if (previus.last_click < key_time_cycle_ms)
        {
            ++previus.n_times;
        }
        break;
    /// We aren't actually interested - all interesting stuff happens in Released and check if code is the same
    case RawKey::State::Pressed:
        break;
    case RawKey::State::LongPress:
        break;
    case RawKey::State::Undefined:
        break;
    }

    return rawKey;
}

InputEvent BaseKeyTrans::translate(RawKey key)
{

    /// TODO TODO - rewrite so that it would make sense - this is
    /// - receive RawKey
    /// - rewrite all handlers?
//    InputEvent( const State& state,
//            const uint32_t& code,
//            const uint32_t& key,
//    		const uint32_t& pressTime,
//            const uint32_t& releaseTime,
//            bool cycle, uint32_t timeout = 0 );

    /// TODO think how to not rewrite too much...

    return InputEvent(gui::InputEvent::State::keyPressed, key.key_code,
                      // TODO TODO - here `standard key translation` instead: keyProfile->chars[n_times],
                      'x', key.duration, 0, key.n_times,
                      // TODO TODO remove it here and handle in app / handle in widget: keyProfile->timeouts[n_times]
                      0);
}

std::vector<std::string> KeyInputTransMapped::getProfilesList(std::string ext)
{

    std::vector<std::string> profileFiles;

    LOG_INFO("Scanning %s profiles folder: %s", ext.c_str(), profilesFolder.c_str());
    auto dirList = vfs.listdir(profilesFolder.c_str(), ext);

    for (vfs::DirectoryEntry ent : dirList)
    {
        if (ent.attributes != vfs::FileAttributes::Directory)
        {
        }
        profileFiles.push_back(profilesFolder + "/" + ent.fileName);
    }

    LOG_INFO("Total number of profiles: %d", profileFiles.size());

    return profileFiles;
}

void KeyInputTransMapped::loadProfile(std::string filepath)
{
    Profile *p = new Profile();
    p->load(filepath);
    // TODO TODO check if profile already exists ...
    profiles.insert(std::pair<std::string, std::shared_ptr<Profile>>(p->getName(), p));
}

bool KeyInputTransMapped::setProfile(std::string profileName)
{
    auto it = profiles.find(profileName);
    if (it == profiles.end())
    {
        currentProfile = nullptr;
        return false;
    }
    currentProfile = it->second;
    return true;
}

bool KeyInputTransMapped::init(std::string baseDirectory)
{
    profilesFolder = baseDirectory + "/profiles";
    std::vector<std::string> profileFiles = getProfilesList(".kprof");

    for (std::string mapName : profileFiles)
    {
        loadProfile(mapName);
    }
    return (profileFiles.size() == profiles.size());
}

} /* namespace gui */
