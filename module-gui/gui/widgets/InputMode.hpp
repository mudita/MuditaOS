#pragma once

#include <functional>
#include <list>

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
    std::list<Mode> input_mode_list;
    uint32_t input_mode_selected = Mode::digit;
    std::function<void(const std::string&)> switch_cb = nullptr;

  public:
    /// method to set mode switching between: abc, ABC, 123
    /// need to be done via callback from application because input mode is parent element and we don't want to mix application in widgets
    InputMode(std::function<void(const std::string&)> switch_cb, Mode starting_mode = Mode::digit, std::list<InputMode::Mode> mode_list = {Mode::digit});

    /// sets selected mode using switch_cb
    bool next();

    /// set selected Mode
    bool setMode(Mode m);
};
