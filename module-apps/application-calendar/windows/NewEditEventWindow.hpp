// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/models/NewEditEventModel.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include "module-apps/application-calendar/data/OptionParser.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/Item.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Text.hpp>
#include <gui/widgets/ListItem.hpp>
#include <ListView.hpp>
#include <Utils.hpp>

namespace gui
{
    class NewEditEventWindow : public gui::AppWindow
    {
      private:
        std::shared_ptr<EventsRecord> eventRecord            = nullptr;
        gui::ListView *list                                  = nullptr;
        std::shared_ptr<NewEditEventModel> newEditEventModel = nullptr;
        EventAction eventAction                              = EventAction::Add;
        void handleEndDateUpdate();
        void checkEventDurationCorrectness();

      public:
        NewEditEventWindow(app::Application *app, std::string name);

        bool handleSwitchData(gui::SwitchData *data) override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace gui */
