// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include <list>
#include <utf8/UTF8.hpp>

/// this element has one goal - nicely change input parsing which is done in application in it's widgets
class InputMode
{
  public:
    /// ! when adding modes please update next method and input_mode (in cpp file)
    enum Mode
    {
        digit,
        Abc,
        abc,
        ABC,
        phone,
    };

  private:
    InputMode() = delete;
    // list of enabled input modes
    std::list<Mode> input_mode_list                    = {};
    std::uint32_t input_mode_list_pos                                                  = 0;
    std::function<void(const UTF8 &text)> show_type_callback                           = nullptr;
    std::function<void()> restore_after_show_type_callback                             = nullptr;
    std::function<void()> show_special_char_selector   = nullptr;
    std::function<void(std::function<void()> restore_function)> restore_timer_callback = nullptr;
    Mode modeNow() const;

    void show_input_type();

  public:
    void show_restore();
    explicit InputMode(std::list<InputMode::Mode> mode_list,
                       std::function<void(const UTF8 &text)> show_type_callback                           = nullptr,
                       std::function<void()> restore_after_show_type_callback                             = nullptr,
                       std::function<void()> show_special_char_selector                                   = nullptr,
                       std::function<void(std::function<void()> restore_function)> restore_timer_callback = nullptr);

    void next();
    const std::string &get();
    const std::string &get(Mode mode);
    void select_special_char();
    [[nodiscard]] bool is(Mode mode) const
    {
        return mode == modeNow();
    }
};
