// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>
#include "Dialog.hpp"

/// Provides information witch window is equivalent to empty window (DayEvents window and AllEvents window may be empty)
enum class EquivalentWindow
{
    DayEventsWindow,
    AllEventsWindow,
    EmptyWindow
};

namespace gui
{
    class Image;

    class NoEvents : public Dialog
    {
        gui::Image *arrow = nullptr;
        gui::Image *cross = nullptr;

      public:
        NoEvents(app::Application *app, const std::string &name);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };

}; // namespace gui
