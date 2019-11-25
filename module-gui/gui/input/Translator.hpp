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
enum class Keymaps {
    ABC,
    abc,
    digit,
};

class KeyBaseTranslation
{
  public:
    RawKey prev_key_press = {};
    InputEvent set(RawKey key);
};

/// KeyPress translator
/// simplest 1:1 keys handling, used when application needs simplest key parsing possible
/// no keys behaviour analysis done here - just mapping
class KeyInputSimpleTranslation : public KeyBaseTranslation
{
  public:
    InputEvent translate(RawKey key);
};

/// translator using & switching KeyMaps for use per widget basis ,called for selected widget, per widget basis
class KeyInputMappedTranslation : public KeyBaseTranslation
{
    uint32_t times = 0;
  public:
    bool setProfile(std::string profileName);
    uint32_t handle(RawKey key, const std::string &keymap);
    uint32_t getTimes() {return times;}
};

} /* namespace gui */
