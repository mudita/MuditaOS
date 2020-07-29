#pragma once

#include "application-calendar/widgets/CalendarStyle.hpp"
#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/Item.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Text.hpp>
#include <gui/widgets/ListItem.hpp>
#include <Utils.hpp>

namespace gui
{
    struct customRepeat
    {
        bool daysChecked[style::window::calendar::week_days_number] = {false};
        bool isNothingChosen()
        {
            for (int i = 0; i < style::window::calendar::week_days_number; ++i) {
                if (daysChecked[i]) {
                    return false;
                }
            }
            return true;
        }
        int dayNameToNumber(std::string &name)
        {
            if (name == utils::localize.get("common_monday")) {
                return 0;
            }
            else if (name == utils::localize.get("common_tuesday")) {
                return 1;
            }
            else if (name == utils::localize.get("common_wednesday")) {
                return 2;
            }
            else if (name == utils::localize.get("common_thursday")) {
                return 3;
            }
            else if (name == utils::localize.get("common_friday")) {
                return 4;
            }
            else if (name == utils::localize.get("common_saturday")) {
                return 5;
            }
            else if (name == utils::localize.get("common_sunday")) {
                return 6;
            }
            else {
                return -1;
            }
        }
    };

    class NewEditEventWindow : public gui::AppWindow
    {
      private:
        enum class EventAction
        {
            None,
            Add,
            Edit
        };
        gui::VBox *body                = nullptr;
        gui::Label *eventNameLabel = nullptr;
        gui::Text *eventNameInput  = nullptr;
        gui::ListItem *checkBox    = nullptr;
        gui::ListItem *startTime   = nullptr;
        gui::ListItem *reminderOptions = nullptr;
        EventAction eventAction        = EventAction::None;

      public:
        NewEditEventWindow(app::Application *app, std::string name);

        // customRepeat *choosenDays = new customRepeat();
        bool handleSwitchData(gui::SwitchData *data) override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
    };

} /* namespace gui */
