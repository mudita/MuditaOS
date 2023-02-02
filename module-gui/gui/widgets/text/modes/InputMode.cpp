// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputMode.hpp"
#include <i18n/i18n.hpp>
#include <map>
#include <log/log.hpp>
#include <utility>

/// input mode strings - as these are stored in json (in files...)
const std::map<InputMode::Mode, std::string> input_mode = {
    {InputMode::digit, "numeric"},
    {InputMode::ABC, "upper"},
    {InputMode::abc, "lower"},
    {InputMode::phone, "phone"},
};

static std::string getInputName(InputMode::Mode m)
{
    switch (m) {
    case InputMode::digit:
        return "123";
    case InputMode::Abc:
        return "Abc";
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
                     std::function<void()> restore_after_show_type_cb,
                     std::function<void()> show_special_char_selector)
    : input_mode_list(std::move(mode_list)), show_type_cb(std::move(show_type_cb)),
      restore_after_show_type_cb(std::move(restore_after_show_type_cb)),
      show_special_char_selector(std::move(show_special_char_selector))
{
    // failsafe
    if (input_mode_list.empty()) {
        input_mode_list.push_back(Mode::digit);
    }
}

InputMode::Mode InputMode::modeNow() const
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
    auto actualInputMode = input_mode.at(modeNow());
    if (actualInputMode == input_mode.find(InputMode::digit)->second ||
        actualInputMode == input_mode.find(InputMode::phone)->second) {
        return input_mode.at(modeNow());
    }
    return utils::getInputLanguageFilename(actualInputMode);
}

const std::string &InputMode::get(InputMode::Mode mode)
{
    const auto &selectedInputMode = input_mode.at(mode);
    return utils::getInputLanguageFilename(selectedInputMode);
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
    if (restore_after_show_type_cb) {
        restore_after_show_type_cb();
    }
}

void InputMode::select_special_char()
{
    LOG_INFO("Special character selector");
    if (show_special_char_selector) {
        show_special_char_selector();
    }
}
