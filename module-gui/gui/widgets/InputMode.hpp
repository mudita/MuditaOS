#pragma once

#include <functional>
#include <list>
#include <utf8/UTF8.hpp>

// TODO read from keymap file ...
namespace gui
{
inline const std::vector<char> special_chars = {'.', ',', '_', ':', ';', ')', '(', '?', '!', '/', '*', '+'};
}

/// this widget has one goal - nicelly change input parsing which is done in application in it's widgets
class InputMode
{
  public:
    /// ! when adding modes please update next method and input_mode (in cpp file)
    enum Mode
    {
        digit,
        abc,
        ABC,
    };

  private:
    InputMode() = delete;
    // list of enabled input modes
    std::list<Mode> input_mode_list = {};
    uint32_t input_mode_list_pos=0;
    std::function<void(const UTF8 &text)> show_type_cb = nullptr;
    // TODO - send KeyPress
    std::function<void()> show_special_char_selector = nullptr;
    Mode modeNow();
    // text to restore when show_input_type was called (and overwritten some other text)
    UTF8 restore_text;

  public:

    InputMode(std::list<InputMode::Mode> mode_list,
            std::function<void(const UTF8 &text)> show_type_cb = nullptr,
            std::function<void()> show_special_char_selector = nullptr,
            const UTF8 &prev_text = ""
            );
    void next();
    const std::string& get();
    void show_input_type();
    void show_restore();
    void select_special_char();
};
