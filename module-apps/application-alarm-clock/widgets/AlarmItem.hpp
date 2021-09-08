// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <widgets/ButtonOnOff.hpp>
#include <ListItem.hpp>
#include <BoxLayout.hpp>
#include <Label.hpp>
#include <application-alarm-clock/presenter/AlarmPresenter.hpp>

namespace gui
{
    class AlarmItem : public ListItem, public app::alarmClock::AlarmEventItem
    {
        gui::HBox *hBox              = nullptr;
        gui::VBox *vBox              = nullptr;
        gui::ButtonOnOff *onOffImage = nullptr;
        gui::Label *timeLabel        = nullptr;
        gui::Label *periodLabel      = nullptr;
        void setAlarm();

      public:
        explicit AlarmItem(app::alarmClock::AlarmPresenter);
    };
} // namespace gui
