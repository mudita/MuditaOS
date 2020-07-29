#include "CalendarMainWindow.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include "application-calendar/models/DayModel.hpp"
#include "application-calendar/models/MonthModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/models/DayEventsModel.hpp"
#include "application-calendar/models/AllEventsModel.hpp"
#include "NoEvents.hpp"
#include <module-services/service-db/messages/QueryMessage.hpp>
#include <module-db/queries/calendar/QueryEventsGetAll.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <time/time_conversion.hpp>

namespace gui
{
    DayLabel::DayLabel(gui::Item *parent,
                       std::string number,
                       const uint32_t &x,
                       const uint32_t &y,
                       const uint32_t &width,
                       const uint32_t &height)
        : Label(parent, 0, 0, 0, 0, ""), DayModel(number, x, y)
    {
        LOG_DEBUG("Call DayLabel constructor. With coords: x:%d, y:%d", static_cast<int>(x), static_cast<int>(y));
        parent->addWidget(this);
        this->setSize(width, height);

        if (y == 0) {
            this->setText(number);
            this->setFont(style::window::font::verysmall);
        }
        this->activeItem = false;
        this->setPenWidth(style::window::default_border_no_focus_w);
        this->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    }

    void DayLabel::setLabel(std::string number, std::function<bool(Item &)> activatedCallback)
    {
        LOG_DEBUG("Set callback of day label. coords: x:%d, y:%d", static_cast<int>(x), static_cast<int>(y));
        this->number     = number;
        this->activeItem = true;
        this->setText(number);
        this->setFont(style::window::font::medium);
        this->activatedCallback = activatedCallback;
        this->setPenWidth(style::window::default_border_no_focus_w);
        this->setPenFocusWidth(style::window::default_border_focus_w);
        this->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_TOP | RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        this->setFont(style::window::font::medium);
    }

    MonthBox::MonthBox(app::Application *app,
                       gui::Item *parent,
                       const int &offsetTop,
                       const uint32_t &width,
                       const uint32_t &height,
                       const uint32_t &dayWidth,
                       const uint32_t &dayHeight,
                       const std::unique_ptr<MonthModel> &model)
        : GridLayout(parent, style::window::default_left_margin, offsetTop, width, height, {dayWidth, dayHeight}),
          MonthModel(model->name, model->days)
    {
        LOG_DEBUG("Call MonthBox constructor");
        // set the costant values of first and first column
        this->firstRow    = 0;
        this->firstColumn = 0;

        assert(parent);
        parent->addWidget(this);

        grid.x        = dayWidth;
        grid.y        = dayHeight;
        this->rows    = style::window::calendar::max_weeks_number;
        this->columns = style::window::calendar::week_days_number;
        for (uint32_t y = 0; y < this->rows; y++) {
            for (uint32_t x = 0; x < this->columns; x++) {
                auto key         = std::make_pair(x, y);
                std::string name = "";
                if (y == 0) {
                    name = utils::time::Locale::get_short_day(x);
                }
                LOG_DEBUG("MonthBox x:%d y:%d", static_cast<int>(x), static_cast<int>(y));
                dayMap[key] = new DayLabel(this,
                                           name,
                                           x,
                                           y,
                                           style::window::calendar::day_cell_width,
                                           style::window::calendar::day_cell_height);
                addWidget(dayMap[key]);
            }
        }
        LOG_DEBUG("MonthBox constructor Completed Successfully!");
    }

    void MonthBox::buildMap(app::ApplicationCalendar *app)
    {
        LOG_DEBUG("Start build month box map");
        for (auto &day : days) {
            if (day.x < this->columns && day.y < this->rows) {
                LOG_DEBUG("Set element in monthbox map in position x:%d y:%d",
                          static_cast<int>(day.x),
                          static_cast<int>(day.y));
                auto key = std::make_pair(day.x, day.y);

                dayMap[key]->setLabel(day.number.c_str(), [=](gui::Item &item) {
                    LOG_DEBUG("Switch to DayEventsWindow");
                    app->switchWindow("DayEventsWindow", nullptr);
                    return true;
                });
            }
            else {
                LOG_DEBUG("COLUMNS:%d ROWS:%d", static_cast<int>(this->columns), static_cast<int>(this->rows));
                LOG_ERROR("Element positioned outside the box. coords x:%d , y:%d",
                          static_cast<int>(day.x),
                          static_cast<int>(day.y));
            }
        }
        LOG_DEBUG("Build Map Completed Successfully!");
    }

    bool CalendarMainWindow::getData(const uint32_t &date_time)
    {
        switch (date_time) {
        case 1: {
            // Test case 1:
            std::vector<DayModel> days;
            uint32_t numbOfWeeks = 4;
            uint32_t daysInWeek  = 7;
            uint32_t startDay    = 0;
            uint32_t numb        = 0;
            for (uint32_t y = 1; y <= numbOfWeeks; y++) {
                for (uint32_t x = 0; x < daysInWeek; x++) {
                    if (y == 1 && x < startDay) {
                        continue;
                    }
                    ++numb;
                    if (numb > 31) {
                        break;
                    }
                    DayModel day(to_string(numb).c_str(), x, y);
                    days.push_back(day);
                }
            }
            this->monthModel = std::make_unique<MonthModel>("July", days);
            return true;
        }
        case 2: {
            // Test case 2:
            std::vector<DayModel> days;
            uint32_t numbOfWeeks = 5;
            uint32_t daysInWeek  = 7;
            uint32_t startDay    = 3;
            uint32_t numb        = 0;
            for (uint32_t y = 1; y <= numbOfWeeks; y++) {
                for (uint32_t x = 0; x < daysInWeek; x++) {
                    if (y == 1 && x < startDay) {
                        continue;
                    }
                    ++numb;
                    if (numb > 28) {
                        break;
                    }
                    DayModel day(to_string(numb).c_str(), x, y);
                    days.push_back(day);
                }
            }
            this->monthModel = std::make_unique<MonthModel>("July", days);
            return true;
        }
        case 3: {
            // Test case 3:
            std::vector<DayModel> days;
            uint32_t numbOfWeeks = 5;
            uint32_t daysInWeek  = 7;
            uint32_t startDay    = 6;
            uint32_t numb        = 0;
            for (uint32_t y = 1; y <= numbOfWeeks; y++) {
                for (uint32_t x = 0; x < daysInWeek; x++) {
                    if (y == 1 && x < startDay) {
                        continue;
                    }
                    ++numb;
                    if (numb > 31) {
                        break;
                    }
                    DayModel day(to_string(numb).c_str(), x, y);
                    days.push_back(day);
                }
            }
            this->monthModel = std::make_unique<MonthModel>("April", days);
            return true;
        }
        }
        return false;
    }

    CalendarMainWindow::CalendarMainWindow(app::Application *app, std::string name) : AppWindow(app, name)
    {
        buildInterface(style::window::calendar::test::month_id);
    }

    void CalendarMainWindow::refresh(const uint32_t &ID, std::string date)
    {
        erase(dateLabel);
        month->erase();
        this->buildMonth(ID);
        this->buildDateLabel(date);
    }

    void CalendarMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface(style::window::calendar::test::month_id);
    }

    void CalendarMainWindow::buildMonth(const uint32_t &actualDateTimeID)
    {
        auto app = dynamic_cast<app::ApplicationCalendar *>(application);
        assert(app != nullptr);

        offsetFromTop = title->offset_h() + style::window::calendar::month_year_height;
        monthWidth    = style::window::default_body_width;
        monthHeight   = style::window_height - title->offset_h() - style::footer::height;
        dayWidth      = style::window::calendar::day_cell_width;
        dayHeight     = style::window::calendar::day_cell_height;

        bool resp = getData(actualDateTimeID);
        LOG_DEBUG("Calendar Date Time ID:%d", static_cast<int>(actualDateTimeID));
        if (resp) {
            // create empty month box
            month = new MonthBox(app, this, offsetFromTop, monthWidth, monthHeight, dayWidth, dayHeight, monthModel);
            // setup month box
            month->buildMap(app);
            addWidget(month);

            month->borderCallback = [this](const InputEvent &inputEvent) -> bool {
                if (inputEvent.state != InputEvent::State::keyReleasedShort) {
                    return false;
                }
                switch (inputEvent.keyCode) {
                case KeyCode::KEY_UP: {
                    LOG_DEBUG("change month prev");
                    this->refresh(style::window::calendar::test::prev_month_id,
                                  style::window::calendar::test::date_text_1);
                    return true;
                }
                case KeyCode::KEY_DOWN: {
                    LOG_DEBUG("change month next");
                    this->refresh(style::window::calendar::test::next_month_id,
                                  style::window::calendar::test::date_text_3);
                    return true;
                }
                case KeyCode::KEY_LEFT: {
                    LOG_DEBUG("Call borderCallback -> go to the previous element");
                    auto it = month->getNavigationFocusedItem();
                    if (month->nextNavigationItem(std::prev(it)) != nullptr) {
                        month->setFocusItem(month->nextNavigationItem(std::prev(it)));
                    }
                    else {
                        month->setFocusOnLastElement();
                    }
                    return true;
                }
                case KeyCode::KEY_RIGHT: {
                    LOG_DEBUG("Call borderCallback -> go to the next element");
                    auto it = month->getNavigationFocusedItem();
                    if (month->nextNavigationItem(std::next(it)) != nullptr) {
                        month->setFocusItem(month->nextNavigationItem(std::next(it)));
                    }
                    else {
                        month->setFocusOnElement(0);
                    }
                    return true;
                }
                default: {
                    return false;
                }
                }
            };

            setFocusItem(month);
        }
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

    void CalendarMainWindow::buildInterface(const uint32_t &actualDateTimeID)
    {
        LOG_DEBUG("AppWindow build interface");
        LOG_DEBUG("Calendar Date Time ID:%d", static_cast<int>(actualDateTimeID));
        AppWindow::buildInterface();

        LOG_DEBUG("Start build interface for calendar main window");
        auto app = dynamic_cast<app::ApplicationCalendar *>(application);
        assert(app != nullptr);

        setTitle(utils::localize.get("app_calendar_title_main"));

        this->buildMonth(actualDateTimeID);
        this->buildDateLabel(style::window::calendar::test::date_text_2);

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get("app_calendar_bar_list"));
        LOG_DEBUG("Calendar Date Time ID:%d", static_cast<int>(actualDateTimeID));
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

        if (inputEvent.keyCode == gui::KeyCode::KEY_ENTER) {
            std::shared_ptr<DayEventsModel> dayEventsModel = std::make_shared<DayEventsModel>(this->application);
            if (dayEventsModel->requestRecordsCount() == 0) {
                switchToNoEventsWindow();
            }
            else {
                LOG_DEBUG("Switch to Day Window");
                application->switchWindow(style::window::calendar::name::day_events_window);
            }
            return true;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_LF) {
            std::shared_ptr<AllEventsModel> allEventsModel = std::make_shared<AllEventsModel>(this->application);
            if (allEventsModel->requestRecordsCount() == 0) {
                switchToNoEventsWindow();
            }
            else {
                LOG_DEBUG("Switch to List Window");
                application->switchWindow(style::window::calendar::name::all_events_window);
            }
            return true;
        }

        return false;
    }

    void CalendarMainWindow::switchToNoEventsWindow()
    {
        auto dialog = dynamic_cast<gui::NoEvents *>(
            this->application->getWindow(style::window::calendar::name::no_events_window));
        assert(dialog != nullptr);
        auto meta   = dialog->meta;
        meta.text   = "app_calendar_no_events_information";
        meta.title  = utils::time::Time().str("%d %B");
        meta.icon   = "phonebook_empty_grey_circle_W_G";
        meta.action = [=]() -> bool {
            LOG_DEBUG("Switch to edit window");
            return true;
        };
        dialog->update(meta);
        this->application->switchWindow(dialog->getName());
        LOG_DEBUG("Switch to no events window");
    }

} // namespace gui
