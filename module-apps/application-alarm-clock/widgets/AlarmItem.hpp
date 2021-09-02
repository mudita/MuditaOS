// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <widgets/ButtonOnOff.hpp>
#include <ListItem.hpp>
#include <BoxLayout.hpp>
#include <Label.hpp>

namespace gui
{
    class AlarmItem : public ListItem
    {
        gui::HBox *hBox              = nullptr;
        gui::VBox *vBox              = nullptr;
        gui::ButtonOnOff *onOffImage = nullptr;
        gui::Label *timeLabel        = nullptr;
        gui::Label *periodLabel      = nullptr;
        std::shared_ptr<AlarmEventRecord> alarm;
        void setAlarm();

      public:
        explicit AlarmItem(std::shared_ptr<AlarmEventRecord> record);
    };
} // namespace gui
