#pragma once

#include "InputEvent.hpp"
#include "Profile.hpp"
#include "bsp/keyboard/key_codes.hpp"
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

/// General description
///
/// [ key input ] -> [ KeyInputHandler ] -> [ SimpleKeyInputHandler - for application ] -> (onInput api) -> [ application simple key handling ]
//                                                                                                  -> [ if exists: translator x for selected widget]
/// key input       - any phisical key change
/// keyInputHandler - gathers key timings ( num of presses, type of press - long, short)
/// translator      - analyses num of presses, type of press it's time and gives final KeyResult
///                   i.e. 3 times clicked 2 is mapped to C, 4 times mapped to A in pl or A umlaut in German

/// key timer - in 1 secod periods, if adjusted - please adjust RawKey duration handling accordingly
const inline uint32_t key_time_base_ms = 1000;
/// default time key press will be counted as press again
const inline uint32_t key_time_cycle_ms = 500;
/// default long press time
const inline uint32_t key_time_longpress_ms = 2000;

namespace gui
{

struct RawKey
{
    /// TODO use the same enum as in InputMessage::State
    enum class State
    {
        Undefined,
        Pressed,
        Released,
        LongPress,
    } state = State::Undefined;

    uint32_t n_times = 0;    /// how many times pressed
    uint32_t duration = 0;   /// how long pressed, in seconds
    uint32_t last_click = 0; /// when was last click
    ///KeyCode key_code = 0;   /// key value pressed
    uint32_t key_code = 0;   /// key value pressed

    /// set all values to zero, set new key_code and state
    void reset(uint32_t key_code, State state) {
        n_times =0; duration = 0; last_click =0;
        this->key_code = key_code;
        this->state = state;
    }
};

/// pre class for input handling
/// gathers key events which will be further packed by translator
class KeyInputHandler
{
  public:
    /// handle new keypress
    /// true  - pressed
    /// false - released
    /// TODO TODO uint32_t to KeyCode ...
    RawKey handle(uint32_t key_code, RawKey::State state, uint32_t timestamp);
    void keyTimeUpdate();

  protected:
    RawKey previus = {RawKey::State::Undefined, 0, 0, 0};

};

class BaseKeyTrans
{
  public:
    RawKey raw;
    virtual InputEvent translate(RawKey key);
};

/// KeyPress translator
/// simplest 1:1 keys handling, used when application needs simplest key parsing possible
/// no keys behaviour analysis done here - just mapping
class KeyInputTransSimple : public BaseKeyTrans
{
  public:
    virtual InputEvent translate(RawKey key) override;
};

/// translator using & switching KeyMaps for use per widget basis ,called for selected widget, per widget basis
class KeyInputTransMapped : public BaseKeyTrans
{
  private:
      /// TODO SET profilesFolder to somve value...
    std::string profilesFolder;
    std::shared_ptr<gui::Profile> currentProfile = nullptr;
    /// TODO check it
    std::map<std::string, std::shared_ptr<gui::Profile>> profiles;
    void loadProfile(std::string filepath);
    std::vector<std::string> getProfilesList(std::string ext);
    const KeyProfile *prevKeyProfile = nullptr;

    bool init(std::string baseDirectory);

  public:
    bool setProfile(std::string profileName);
    virtual InputEvent translate(RawKey key) override;
};

} /* namespace gui */
