// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <OptionWindow.hpp>

namespace gui
{
    ///  @brief Helping class for easy options creation/manipulation
    class OptionsWidgetMaker
    {
      public:
        ///  Constructor
        ///  @param window pointer to parent option window
        explicit OptionsWidgetMaker(OptionWindow *window);

        ///  Adds switch option to the given options list
        ///  @param list list to be updated
        ///  @param text option description text
        ///  @param option state of option
        ///  @param callback called whenever option is changed
        ///  @param indent should the option be indent
        void addSwitchOption(std::list<gui::Option> &list,
                             const UTF8 &text,
                             bool &option,
                             const std::function<void()> &callback,
                             bool indent = false);

        ///  Adds select option to the given options list
        ///  @param list list to be updated
        ///  @param text option description text
        ///  @param callback called whenever option is clicked
        ///  @param indent should the option be indent
        void addSelectOption(std::list<gui::Option> &list,
                             const UTF8 &text,
                             const std::function<void(void)> &callback,
                             bool indent = false);

      private:
        ///  Pointer to the window containing the options
        OptionWindow *mWindow = nullptr;
    };
} // namespace gui
