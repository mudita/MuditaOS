#include "DayEventsWindow.hpp"

#include <gui/widgets/Window.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Item.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/TopBar.hpp>

#include <time/time_conversion.hpp>
#include <module-services/service-db/messages/QueryMessage.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>
#include <module-db/queries/calendar/QueryEventsGetAll.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <module-apps/application-calendar/ApplicationCalendar.hpp>

namespace gui
{

    DayEventsWindow::DayEventsWindow(app::Application *app, std::string name)
        : AppWindow(app, style::window::calendar::name::day_events_window),
          dayEventsModel{std::make_shared<DayEventsModel>(this->application)}
    {
        buildInterface();
    }

    void DayEventsWindow::rebuild()
    {
        buildInterface();
    }

    void DayEventsWindow::buildInterface()
    {
        AppWindow::buildInterface();

        auto ttime = utils::time::Time();
        topBar->setActive(gui::TopBar::Elements::TIME, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));

        setTitle(ttime.str("%d %B"));
        leftArrowImage = new gui::Image(
            this, style::window::calendar::arrow_x, style::window::calendar::arrow_y, 0, 0, "arrow_left");
        newDayEventImage =
            new gui::Image(this, style::window::calendar::cross_x, style::window::calendar::cross_y, 0, 0, "cross");

        dayEventsList = new gui::ListView(this,
                                          style::window::calendar::listView_x,
                                          style::window::calendar::listView_y,
                                          style::window::calendar::listView_w,
                                          style::window::calendar::listView_h,
                                          dayEventsModel);

        dayEventsList->setPenFocusWidth(style::window::default_border_no_focus_w);
        dayEventsList->setPenWidth(style::window::default_border_no_focus_w);

        setFocusItem(dayEventsList);
    }

    bool DayEventsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_LEFT) {
            LOG_DEBUG("Switch to new window - edit window");
            auto msg = DBServiceAPI::GetQueryWithReply(
                application, db::Interface::Name::Events, std::make_unique<db::query::events::GetAll>(), 1000);

            LOG_DEBUG("Type id %s", typeid(*msg.second).name());
            auto msgl = msg.second.get();
            assert(msgl != nullptr);
            onDatabaseMessage(msgl);
            return true;
        }
        return false;
    }

    bool DayEventsWindow::onDatabaseMessage(sys::Message *msgl)
    {
        auto msg = dynamic_cast<db::QueryResponse *>(msgl);
        if (msg != nullptr) {
            auto temp = msg->getResult();
            if (auto response = dynamic_cast<db::query::events::GetAllResult *>(temp.get())) {
                unique_ptr<vector<EventsRecord>> records = response->getResult();
                for (auto &rec : *records) {
                    LOG_DEBUG("record: %s", rec.title.c_str());
                }
                uint32_t numberOfItems = records->size();
                dayEventsModel->setRecordsCount(numberOfItems);
                return dayEventsModel->updateRecords(std::move(records));
            }
            LOG_DEBUG("Response False");
            return false;
        }
        LOG_DEBUG("DayWindow DB Message != QueryResponse");
        return false;
    }
} /* namespace gui */
