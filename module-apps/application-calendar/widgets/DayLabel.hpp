// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Application.hpp>
#include <gui/widgets/BoxLayout.hpp>

namespace gui
{
    class Image;

    class DayLabel : public Rect
    {
        gui::VBox *vBox       = nullptr;
        gui::Label *dayNumber = nullptr;
        gui::Image *dotImage  = nullptr;

      public:
        DayLabel(app::Application *app,
                 gui::Item *parent,
                 const uint32_t &cellIndex,
                 const uint32_t &firstWeekOffset,
                 const uint32_t &width,
                 const uint32_t &height,
                 bool isDayEmpty);
        ~DayLabel() override = default;

        uint32_t getDayNumber();
    };

} /* namespace gui */
