// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-calendar/models/DateTimeModel.hpp"
#include "Application.hpp"
#include <ListView.hpp>
#include <module-db/Interface/EventsRecord.hpp>

namespace gui
{
    class CalendarDateTimeWindow : public gui::AppWindow
    {
        std::shared_ptr<EventsRecord> eventRecord;
        std::shared_ptr<DateTimeModel> dateTimeModel;
        gui::ListView *list       = nullptr;
        DateTimeType dateTimeType = DateTimeType::Start;

      public:
        explicit CalendarDateTimeWindow(app::Application *app);

        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace gui */
