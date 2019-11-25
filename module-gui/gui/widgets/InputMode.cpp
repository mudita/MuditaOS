#include <InputMode.hpp>
#include <i18/i18.hpp>
#include <map>

/// input mode strings - as these are stored in json (in files...)
const std::map<InputMode::Mode, std::string> input_mode = {
    {InputMode::digit, "numeric"},
    {InputMode::ABC, "common_kbd_upper"},
    {InputMode::abc, "common_kbd_lower"},
};

InputMode::InputMode(std::list<InputMode::Mode> mode_list)
    : input_mode_list(mode_list)
{
    // failsafe
    if(input_mode_list.size() == 0) {
        input_mode_list.push_back(Mode::digit);
    }
}

/// sets next selected mode using Application pointer
void InputMode::next()
{
    ++input_mode_list_pos;
    if (input_mode_list_pos == input_mode_list.size())
    {
        input_mode_list_pos = input_mode_list.front();
    }
}

const std::string& InputMode::get()
{
    return utils::localize.get(input_mode.at(*std::next(input_mode_list.begin(),input_mode_list_pos)));
}
