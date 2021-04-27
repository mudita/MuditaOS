// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalendarMainWindow.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include "application-calendar/models/MonthModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "NoEvents.hpp"
#include <module-db/queries/calendar/QueryEventsGetAll.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>
#include <time/time_conversion.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <module-db/Common/Query.hpp>

namespace gui
{

    CalendarMainWindow::CalendarMainWindow(app::Application *app, const std::string &name)
        : AppWindow(app, name), app::AsyncCallbackReceiver{app}
    {
        auto appCalendar = dynamic_cast<app::ApplicationCalendar *>(application);
        assert(appCalendar != nullptr);
        std::chrono::system_clock::time_point tp =
            std::chrono::system_clock::from_time_t(appCalendar->getCurrentTimeStamp());
        this->actualDate = date::year_month_day{date::floor<date::days>(tp)};
        std::fill(std::begin(isDayEmpty), std::end(isDayEmpty), true);
        buildInterface();
    }

    void CalendarMainWindow::refresh(const std::vector<EventsRecord> &records)
    {
        erase(dateLabel);
        monthBox->erase();
        monthModel           = std::make_unique<MonthModel>(actualDate);
        monthModel->markEventsInDays(records, isDayEmpty);
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
            if (!inputEvent.isShortRelease()) {
                return false;
            }
            switch (inputEvent.getKeyCode()) {
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
                auto it = std::prev(monthBox->getNavigationFocusedItem());
                if (it != monthBox->children.end() && (*it)->isActive()) {
                    monthBox->setFocusItem(*it);
                }
                else {
                    monthBox->setFocusOnLastElement();
                }
                return true;
            }
            case KeyCode::KEY_RIGHT: {
                LOG_DEBUG("Call borderCallback -> go to the next element");
                auto it = std::next(monthBox->getNavigationFocusedItem());
                if (it != monthBox->children.end() && (*it)->isActive()) {
                    monthBox->setFocusItem(*it);
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

        setTitle(utils::translate("app_calendar_title_main"));

        monthModel = std::make_unique<MonthModel>(actualDate);
        filterRequest();
        this->buildMonth(monthModel);
        this->buildDateLabel(monthModel->getMonthYearText());

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(style::strings::common::open));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::translate("app_calendar_bar_list"));
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

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_LF)) {
            auto app = dynamic_cast<app::ApplicationCalendar *>(application);
            assert(application != nullptr);
            app->setEquivalentToEmptyWindow(EquivalentWindow::AllEventsWindow);
            auto query = std::make_unique<db::query::events::GetAll>();
            auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Events);
            task->setCallback([this](auto response) { return handleQueryResponse(response); });
            task->execute(application, this);
            return true;
        }

        return false;
    }

    void CalendarMainWindow::filterRequest()
    {
        calendar::YearMonthDay date_from = actualDate.year() / actualDate.month() / 1;
        calendar::YearMonthDay date_till = date_from + date::months{1};
        auto filter_from       = TimePointFromYearMonthDay(date_from);
        auto filter_till       = TimePointFromYearMonthDay(date_till);
        LOG_DEBUG("filter:  %s", TimePointToString(filter_till).c_str());
        auto query = std::make_unique<db::query::events::GetFiltered>(filter_from, filter_till);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Events);
        task->setCallback([this](auto response) { return handleQueryResponse(response); });
        task->execute(application, this);
    }

    auto CalendarMainWindow::handleQueryResponse(db::QueryResult *queryResult) -> bool
    {
        std::fill(std::begin(isDayEmpty), std::end(isDayEmpty), true);
        if (auto response = dynamic_cast<db::query::events::GetFilteredResult *>(queryResult)) {
            const auto records = response->getResult();
            refresh(records);
            return true;
        }
        if (auto response = dynamic_cast<db::query::events::GetAllResult *>(queryResult)) {
            const auto records = response->getResult();
            auto day           = monthBox->getFocusItemIndex() + 1;
            auto filter = TimePointFromYearMonthDay(monthModel->getYear() / monthModel->getMonth() / date::day(day));
            if (!records.empty()) {
                auto data = std::make_unique<DayMonthData>();
                data->setData("", filter);
                application->switchWindow(style::window::calendar::name::all_events_window, std::move(data));
            }
            else {
                auto appCalendar = dynamic_cast<app::ApplicationCalendar *>(application);
                assert(appCalendar != nullptr);
                appCalendar->switchToNoEventsWindow(utils::translate("app_calendar_title_main"), filter);
            }
            return true;
        }
        LOG_DEBUG("Response False");
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
