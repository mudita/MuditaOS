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
        enum class EventAction
        {
            None,
            Add,
            Edit
        };
        std::string prevWindowName                           = "";
        std::shared_ptr<EventsRecord> eventRecord            = nullptr;
        gui::ListView *list                                  = nullptr;
        std::shared_ptr<NewEditEventModel> newEditEventModel = nullptr;
        EventAction eventAction                              = EventAction::None;

      public:
        NewEditEventWindow(app::Application *app, std::string name);

        bool handleSwitchData(gui::SwitchData *data) override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace gui */
