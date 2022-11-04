// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <widgets/ButtonTriState.hpp>
#include <ListItem.hpp>
#include <BoxLayout.hpp>
#include <Label.hpp>
#include <application-alarm-clock/presenter/AlarmRRulePresenter.hpp>

namespace gui
{
    class AlarmItem : public ListItem, public app::alarmClock::AlarmRRuleItem
    {
        gui::HBox *hBox              = nullptr;
        gui::VBox *vBox              = nullptr;
        gui::ButtonTriState *onOffImage = nullptr;
        gui::Label *timeLabel        = nullptr;
        gui::Label *periodLabel      = nullptr;
        void setAlarm();

      public:
        explicit AlarmItem(std::shared_ptr<app::alarmClock::AlarmRRulePresenter>);
    };
} // namespace gui
