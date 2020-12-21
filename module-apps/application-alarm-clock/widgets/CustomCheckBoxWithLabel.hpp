// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmInternalListItem.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include "Application.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include <BoxLayout.hpp>
#include <CheckBox.hpp>

namespace gui
{
    class CustomCheckBoxWithLabel : public AlarmInternalListItem
    {
        gui::HBox *hBox               = nullptr;
        app::Application *application = nullptr;
        gui::Label *descriptionLabel  = nullptr;
        gui::CheckBox *checkBox       = nullptr;
        WeekDaysRepeatData checkBoxData;

        void setCheckBoxes();
        void applyCallbacks();

      public:
        CustomCheckBoxWithLabel(app::Application *app, const std::string &description, const WeekDaysRepeatData &data);

        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;

        static const std::map<WeekDayIso, std::string> weekDays;
    };
} // namespace gui
