#include <InputMode.hpp>
#include <i18/i18.hpp>
#include <map>

/// input mode strings - as these are stored in json (in files...)
const std::map<InputMode::Mode, std::string> input_mode = {
    {InputMode::digit, "numeric"},
    {InputMode::ABC, "common_kbd_upper"},
    {InputMode::abc, "common_kbd_lower"},
};

InputMode::InputMode(InputMode::Mode starting_mode, std::list<InputMode::Mode> mode_list)
    : input_mode_selected(starting_mode), input_mode_list(mode_list)
{
    // failsafe
    if (input_mode_selected < 0 || input_mode_selected > Mode::ABC)
    {
        input_mode_selected = Mode::digit;
    }
    set(Mode(input_mode_selected));
}

/// sets next selected mode using Application pointer
bool InputMode::next()
{
    LOG_INFO("set next mode!");
    ++input_mode_selected;
    if (input_mode_selected > Mode::ABC)
    {
        input_mode_selected = Mode::digit;
    }
    return set(Mode(input_mode_selected));
}


const std::string& InputMode::get()
{
    return utils::localize.get(input_mode.at(Mode(input_mode_selected)));
}

bool InputMode::set(InputMode::Mode m)
{
    // mode not in mode list
    if (std::find(input_mode_list.begin(), input_mode_list.end(), m) == input_mode_list.end())
    {
        return false;
    }
    return true;
}
