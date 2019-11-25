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
    std::list<Mode> input_mode_list = {};
    uint32_t input_mode_list_pos=0;

  public:

    InputMode(std::list<InputMode::Mode> mode_list);
    void next();
    const std::string& get();
};
