#include "Translator.hpp"
#include "log/log.hpp"
#include "vfs.hpp"
#include <algorithm>

namespace gui
{

void recon_long_press(InputEvent &evt, const RawKey &key, const RawKey &prev_key_press, uint32_t time)
{
    if (key.state == RawKey::State::Released && prev_key_press.key_code == key.key_code)
    {
        // determine long press
        if (key.time_release - prev_key_press.time_press >= key_time_longpress_ms)
        {
            evt.state = InputEvent::State::keyReleasedLong;
        }
    }
}

InputEvent KeyBaseTranslation::set(RawKey key)
{
    gui::InputEvent evt(key);
    if (key.state == RawKey::State::Pressed)
    {
        evt.state = InputEvent::State::keyPressed;
    }
    else if (key.state == RawKey::State::Released)
    {
        evt.state = InputEvent::State::keyReleasedShort;
    }
    recon_long_press(evt, key, prev_key_press, key_time_longpress_ms);
    // store last key press/release
    if (key.state == RawKey::State::Pressed)
    {
        prev_key_press = key;
    }
    return evt;
}

InputEvent KeyInputSimpleTranslation::translate(RawKey key)
{
    auto evt = KeyBaseTranslation::set(key);

    switch(key.key_code) {
        case bsp::KeyCodes::NumericKey1: evt.keyCode = gui::KeyCode::KEY_1; break;
        case bsp::KeyCodes::NumericKey2: evt.keyCode = gui::KeyCode::KEY_2; break;
        case bsp::KeyCodes::NumericKey3: evt.keyCode = gui::KeyCode::KEY_3; break;
        case bsp::KeyCodes::NumericKey4: evt.keyCode = gui::KeyCode::KEY_4; break;
        case bsp::KeyCodes::NumericKey5: evt.keyCode = gui::KeyCode::KEY_5; break;
        case bsp::KeyCodes::NumericKey6: evt.keyCode = gui::KeyCode::KEY_6; break;
        case bsp::KeyCodes::NumericKey7: evt.keyCode = gui::KeyCode::KEY_7; break;
        case bsp::KeyCodes::NumericKey8: evt.keyCode = gui::KeyCode::KEY_8; break;
        case bsp::KeyCodes::NumericKey9: evt.keyCode = gui::KeyCode::KEY_9; break;
        case bsp::KeyCodes::NumericKey0: evt.keyCode = gui::KeyCode::KEY_0; break;
        case bsp::KeyCodes::NumericKeyAst: evt.keyCode = gui::KeyCode::KEY_AST; break;
        case bsp::KeyCodes::NumericKeyPnd: evt.keyCode = gui::KeyCode::KEY_PND; break;
        case bsp::KeyCodes::JoystickLeft: evt.keyCode = gui::KeyCode::KEY_LEFT; break;
        case bsp::KeyCodes::JoystickRight: evt.keyCode = gui::KeyCode::KEY_RIGHT; break;
        case bsp::KeyCodes::JoystickUp: evt.keyCode = gui::KeyCode::KEY_UP; break;
        case bsp::KeyCodes::JoystickDown: evt.keyCode = gui::KeyCode::KEY_DOWN; break;
        case bsp::KeyCodes::JoystickEnter: evt.keyCode = gui::KeyCode::KEY_ENTER; break;
        case bsp::KeyCodes::FnLeft: evt.keyCode = gui::KeyCode::KEY_LF; break;
        case bsp::KeyCodes::FnRight: evt.keyCode = gui::KeyCode::KEY_RF; break;
        case bsp::KeyCodes::VolUp: evt.keyCode = gui::KeyCode::KEY_VOLUP; break;
        case bsp::KeyCodes::VolDown: evt.keyCode = gui::KeyCode::KEY_VOLDN; break;
        case bsp::KeyCodes::Torch: evt.keyCode = gui::KeyCode::KEY_TORCH; break;
        case bsp::KeyCodes::SSwitchUp: evt.keyCode = gui::KeyCode::SWITCH_UP; break;
        case bsp::KeyCodes::SSwitchDown: evt.keyCode = gui::KeyCode::SWITCH_DN; break;
        case bsp::KeyCodes::SSwitchMid: evt.keyCode = gui::KeyCode::SWITCH_MID; break;
        default:
            LOG_ERROR("Unhandled bsp key!");
    }

    return evt;
}

/// profiles cache - load once for all
class Profiles
{
  private:
    const char* profilesFolder = "sys/assets/profiles";
    std::map<std::string, gui::Profile> profiles = {};

    void loadProfile(const std::string &filepath)
    {
        LOG_INFO("Load profile: %s", filepath.c_str());
        auto p = Profile(filepath);
        if( p.getName() != std::string() ) {
            profiles.insert({p.getName(), std::move(p)});
        }
    }

    std::vector<std::string> getProfilesList(std::string ext)
    {
        std::vector<std::string> profileFiles;
        LOG_INFO("Scanning %s profiles folder: %s", ext.c_str(), profilesFolder);
        auto dirList = vfs.listdir(profilesFolder, ext);

        for (vfs::DirectoryEntry ent : dirList)
        {
            if (ent.attributes != vfs::FileAttributes::Directory)
            {
                profileFiles.push_back(std::string(profilesFolder) + "/" + ent.fileName);
            }
        }

        LOG_INFO("Total number of profiles: %d", profileFiles.size());
        return profileFiles;
    }

    void init()
    {
        std::vector<std::string> profileFiles = getProfilesList(".kprof");
        for (std::string mapName : profileFiles)
        {
            if(std::size(mapName)) {
                loadProfile(mapName);
            }
        }
        if( std::size(profiles) == 0) {
            LOG_ERROR("No keyboard profiles loaded");
        }
    }
    Profile empty;

  public:
    static Profiles &get()
    {
        static Profiles *p;
        if (p == nullptr)
        {
            p = new Profiles();
            p->init();
        }
        return *p;
    }

    static Profile &get(const std::string &name)
    {
        // if profile not in profile map -> load
        if(std::size(name) ==0) {
            LOG_ERROR("Request for non existend profile: %s", name.c_str());
            return get().empty;
        }
        if (get().profiles.find(name) == get().profiles.end())
        {
            get().loadProfile(name);
        }
        return get().profiles[name];
    }
};

uint32_t KeyInputMappedTranslation::handle(RawKey key, const std::string &keymap)
{
    // get shortpress
    if (prev_key_press.key_code != key.key_code)
    {
        times = 0;
    }
    else if (key.state == RawKey::State::Released)
    {
        /// TODO use key_time_cycle_ms from keymap (if exists in keymap...)
        if (key.time_release - prev_key_press.time_release < key_time_cycle_ms)
        {
            ++times;
        }
        else
        {
            times = 0;
        }
    }
    if (key.state == RawKey::State::Released)
    {
        prev_key_press = key;
    }

    return Profiles::get(keymap).get(key.key_code, times);
}

} /* namespace gui */
