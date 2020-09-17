#include "CalendarMainWindow.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include "application-calendar/models/MonthModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "NoEvents.hpp"
#include <module-services/service-db/messages/QueryMessage.hpp>
#include <module-db/queries/calendar/QueryEventsGetAll.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <time/time_conversion.hpp>

namespace gui
{

    CalendarMainWindow::CalendarMainWindow(app::Application *app, std::string name) : AppWindow(app, name)
    {
        auto appCalendar = dynamic_cast<app::ApplicationCalendar *>(application);
        assert(appCalendar != nullptr);
        std::chrono::system_clock::time_point tp =
            std::chrono::system_clock::from_time_t(appCalendar->getCurrentTimeStamp());
        this->actualDate = date::year_month_day{date::floor<date::days>(tp)};
        std::fill(begin(isDayEmpty), end(isDayEmpty), true);
        buildInterface();
    }

    void CalendarMainWindow::refresh()
    {
        erase(dateLabel);
        monthBox->erase();

        monthModel           = std::make_unique<MonthModel>(actualDate);
        std::string dateText = monthModel->getMonthYearText();
        this->buildMonth(monthModel);
        this->buildDateLabel(dateText);
    }

    void CalendarMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void CalendarMainWindow::buildMonth(std::unique_ptr<MonthModel> &model)
    {
        auto app = dynamic_cast<app::ApplicationCalendar *>(application);
        assert(app != nullptr);

        offsetFromTop = title->offset_h() + style::window::calendar::month_year_height;
        monthWidth    = style::window::default_body_width;
        monthHeight   = style::window_height - title->offset_h() - style::footer::height;
        dayWidth      = style::window::calendar::day_cell_width;
        dayHeight     = style::window::calendar::day_cell_height;

        // create empty month box
        monthBox = new MonthBox(
            app, this, offsetFromTop, monthWidth, monthHeight, dayWidth, dayHeight, monthModel, isDayEmpty);
        // setup month box
        addWidget(monthBox);

        monthBox->borderCallback = [this](const InputEvent &inputEvent) -> bool {
            if (inputEvent.state != InputEvent::State::keyReleasedShort) {
                return false;
            }
            switch (inputEvent.keyCode) {
            case KeyCode::KEY_UP: {
                LOG_DEBUG("change month prev");
                if (actualDate.month() != date::January) {
                    date::month prevMonth = --actualDate.month();
                    actualDate            = actualDate.year() / prevMonth / date::last;
                }
                else {
                    date::month prevMonth = date::December;
                    date::year prevYear   = --actualDate.year();
                    actualDate            = prevYear / prevMonth / date::last;
                }

                filterRequest();
                return true;
            }
            case KeyCode::KEY_DOWN: {
                LOG_DEBUG("change month next");
                if (actualDate.month() != date::December) {
                    date::month nextMonth = ++actualDate.month();
                    actualDate            = actualDate.year() / nextMonth / 1;
                }
                else {
                    date::month nextMonth = date::January;
                    date::year nextYear   = ++actualDate.year();
                    actualDate            = nextYear / nextMonth / 1;
                }

                filterRequest();
                return true;
            }
            case KeyCode::KEY_LEFT: {
                LOG_DEBUG("Call borderCallback -> go to the previous element");
                auto it = monthBox->getNavigationFocusedItem();
                if (monthBox->nextNavigationItem(std::prev(it)) != nullptr) {
                    monthBox->setFocusItem(monthBox->nextNavigationItem(std::prev(it)));
                }
                else {
                    monthBox->setFocusOnLastElement();
                }
                return true;
            }
            case KeyCode::KEY_RIGHT: {
                LOG_DEBUG("Call borderCallback -> go to the next element");
                auto it = monthBox->getNavigationFocusedItem();
                if (monthBox->nextNavigationItem(std::next(it)) != nullptr) {
                    monthBox->setFocusItem(monthBox->nextNavigationItem(std::next(it)));
                }
                else {
                    monthBox->setFocusOnElement(0);
                }
                return true;
            }
            default: {
                return false;
            }
            }
        };

        setFocusItem(monthBox);
    }

    void CalendarMainWindow::buildDateLabel(std::string actualDateTime)
    {
        dateLabel = new Label(this,
                              style::window::default_left_margin,
                              title->offset_h(),
                              style::window::default_body_width,
                              style::window::calendar::month_year_height,
                              actualDateTime);

        dateLabel->setPenWidth(style::window::default_border_no_focus_w);
        dateLabel->setFont(style::window::font::mediumbold);
        dateLabel->setAlignment(gui::Alignment(gui::Alignment::Vertical::Center));
        addWidget(dateLabel);
    }

    void CalendarMainWindow::buildInterface()
    {
        LOG_DEBUG("AppWindow build interface");
        AppWindow::buildInterface();

        LOG_DEBUG("Start build interface for calendar main window");

        setTitle(utils::localize.get("app_calendar_title_main"));

        monthModel = std::make_unique<MonthModel>(actualDate);
        filterRequest();
        this->buildMonth(monthModel);
        this->buildDateLabel(monthModel->getMonthYearText());

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get("app_calendar_bar_list"));
    }

    void CalendarMainWindow::destroyInterface()
    {
        erase();
    }

    bool CalendarMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_LF) {
            DBServiceAPI::GetQuery(
                application, db::Interface::Name::Events, std::make_unique<db::query::events::GetAll>());
            return true;
        }

        return false;
    }

    void CalendarMainWindow::filterRequest()
    {
        YearMonthDay date_from = actualDate.year() / actualDate.month() / 1;
        YearMonthDay date_till = date_from + date::months{1};
        auto filter_from       = TimePointFromYearMonthDay(date_from);
        auto filter_till       = TimePointFromYearMonthDay(date_till);
        LOG_DEBUG("filter:  %s", TimePointToString(filter_till).c_str());
        DBServiceAPI::GetQuery(application,
                               db::Interface::Name::Events,
                               std::make_unique<db::query::events::GetFiltered>(filter_from, filter_till));
    }

    bool CalendarMainWindow::onDatabaseMessage(sys::Message *msgl)
    {
        std::fill(begin(isDayEmpty), end(isDayEmpty), true);
        auto msg = dynamic_cast<db::QueryResponse *>(msgl);
        if (msg != nullptr) {
            auto temp = msg->getResult();
            if (auto response = dynamic_cast<db::query::events::GetFilteredResult *>(temp.get())) {
                unique_ptr<vector<EventsRecord>> records = response->getResult();
                for (auto &rec : *records) {
                    date::year_month_day recordDate = TimePointToYearMonthDay(rec.date_from);
                    uint32_t dayNumb                = static_cast<unsigned>(recordDate.day());
                    isDayEmpty[dayNumb - 1]         = false;
                }
                refresh();
                return true;
            }
            if (auto response = dynamic_cast<db::query::events::GetAllResult *>(temp.get())) {
                unique_ptr<vector<EventsRecord>> records = response->getResult();
                if (records->size() != 0) {
                    application->switchWindow(style::window::calendar::name::all_events_window);
                }
                else {
                    auto appCalendar = dynamic_cast<app::ApplicationCalendar *>(application);
                    assert(appCalendar != nullptr);
                    auto filter = TimePointFromYearMonthDay(actualDate);
                    appCalendar->switchToNoEventsWindow(utils::localize.get("app_calendar_title_main"),
                                                        filter,
                                                        style::window::calendar::name::all_events_window);
                }
                return true;
            }
            LOG_DEBUG("Response False");
            return false;
        }
        LOG_DEBUG("Calendar MainWindow DB Message != QueryResponse");
        return false;
    }

    void CalendarMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_RETURN) {
            returnedFromWindow = true;
            auto day           = dynamic_cast<DayLabel *>(monthBox->getFocusItem());
            dayFocusedBefore   = day->getDayNumber();
        }
        filterRequest();
    }

} // namespace gui
