// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalendarListItem.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "Application.hpp"
#include <BoxLayout.hpp>
#include <Label.hpp>
#include <Image.hpp>

namespace gui
{
    class DateTimeItem : public CalendarListItem
    {
        app::Application *app     = nullptr;
        gui::VBox *vBox           = nullptr;
        gui::HBox *hBox           = nullptr;
        gui::Label *description   = nullptr;
        gui::Label *dateTime      = nullptr;
        gui::Image *image         = nullptr;
        DateTimeType dateTimeType = DateTimeType::Start;

        void buildInterface();
        void applyCallbacks();
        std::string getSpecificDateTimeFormat(TimePoint time);

      public:
        DateTimeItem(app::Application *app, DateTimeType dateTimeType);
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };
} // namespace gui
