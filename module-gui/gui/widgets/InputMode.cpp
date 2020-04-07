#include <InputMode.hpp>
#include <i18/i18.hpp>
#include <map>

/// input mode strings - as these are stored in json (in files...)
const std::map<InputMode::Mode, std::string> input_mode = {
    {InputMode::digit, "common_kbd_numeric"},
    {InputMode::ABC, "common_kbd_upper"},
    {InputMode::abc, "common_kbd_lower"},
    {InputMode::phone, "common_kbd_phone"},
};

static std::string getInputName(InputMode::Mode m)
{
    switch (m) {
    case InputMode::digit:
        return "123";
    case InputMode::ABC:
        return "ABC";
    case InputMode::abc:
        return "abc";
    case InputMode::phone:
        return "phone";
    default:
        return "";
    }
}

InputMode::InputMode(std::list<InputMode::Mode> mode_list,
                     std::function<void(const UTF8 &text)> show_type_cb,
                     std::function<void()> show_special_char_selector,
                     const UTF8 &prev_text)
    : input_mode_list(mode_list), show_type_cb(show_type_cb), show_special_char_selector(show_special_char_selector)
{
    // failsafe
    if (input_mode_list.size() == 0) {
        input_mode_list.push_back(Mode::digit);
    }
}

InputMode::Mode InputMode::modeNow()
{
    return *std::next(input_mode_list.begin(), input_mode_list_pos);
}

/// sets next selected mode using Application pointer
void InputMode::next()
{
    ++input_mode_list_pos;
    if (input_mode_list_pos == input_mode_list.size()) {
        input_mode_list_pos = 0;
    }
    LOG_INFO("%" PRIu32, input_mode_list_pos);
    show_input_type();
}

const std::string &InputMode::get()
{
    return utils::localize.get(input_mode.at(modeNow()));
}

void InputMode::show_input_type()
{
    LOG_INFO("Mode: %d", modeNow());
    if (show_type_cb) {
        show_type_cb(getInputName(modeNow()));
    }
}

void InputMode::show_restore()
{
    if (show_type_cb) {
        show_type_cb(restore_text);
    }
}

void InputMode::select_special_char()
{
    LOG_INFO("Special character selector");
    if (show_special_char_selector) {
        show_special_char_selector();
    }
}
