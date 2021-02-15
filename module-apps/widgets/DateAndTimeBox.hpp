// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "widgets/DateWidget.hpp"
#include "widgets/TimeWidget.hpp"
#include <CheckBox.hpp>

namespace gui
{
    class DateAndTimeBox : public VBox
    {
      public:
        explicit DateAndTimeBox(app::Application *application, Item *parent);

        [[nodiscard]] DateWidget *getDateWidget() const noexcept
        {
            return dateWidget;
        }

        std::function<void(std::shared_ptr<EventsRecord> event)> onLoadCallback = nullptr;
        std::function<bool(std::shared_ptr<EventsRecord> event)> onSaveCallback = nullptr;

        // private:
        app::Application *app       = nullptr;
        DateWidget *dateWidget      = nullptr;
        TimeWidget *endTimeWidget   = nullptr;
        TimeWidget *startTimeWidget = nullptr;
        gui::VBox *vBox             = nullptr;
        Item *savedFocusItem        = nullptr;
    };
} /* namespace gui */
