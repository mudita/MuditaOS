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
    DayLabel::DayLabel(app::Application *app,
                       gui::Item *parent,
                       const uint32_t &i,
                       const uint32_t &firstWeekOffset,
                       const uint32_t &width,
                       const uint32_t &height)
        : Label(parent, 0, 0, 0, 0, "")
    {
        LOG_DEBUG("Call DayLabel constructor");

        parent->addWidget(this);
        this->setSize(width, height);

        if (i < style::window::calendar::week_days_number) {
            this->setText(utils::time::Locale::get_short_day(i));
            this->setFont(style::window::font::verysmall);
            this->setPenWidth(style::window::default_border_no_focus_w);
            this->activeItem = false;
        }
        else if (i >= style::window::calendar::week_days_number &&
                 i < style::window::calendar::week_days_number + firstWeekOffset - 1) {
            this->setPenWidth(style::window::default_border_no_focus_w);
            this->activeItem = false;
        }
        else {
            std::string number = std::to_string(i - firstWeekOffset - style::window::calendar::week_days_number + 2);
            this->setText(number);
            this->activeItem   = true;
            this->setFont(style::window::font::medium);
            this->activatedCallback = [=](gui::Item &item) {
                LOG_DEBUG("Switch to DayEventsWindow");
                app->switchWindow("DayEventsWindow", nullptr);
                return true;
            };
            this->setPenWidth(style::window::default_border_no_focus_w);
            this->setPenFocusWidth(style::window::default_border_focus_w);
            this->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_TOP | RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
            this->setFont(style::window::font::medium);
        }
        this->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    }

    MonthBox::MonthBox(app::Application *app,
                       gui::Item *parent,
                       const int &offsetTop,
                       const uint32_t &width,
                       const uint32_t &height,
                       const uint32_t &dayWidth,
                       const uint32_t &dayHeight,
                       const std::unique_ptr<MonthModel> &model)
        : GridLayout(parent, style::window::default_left_margin, offsetTop, width, height, {dayWidth, dayHeight})
    {
        LOG_DEBUG("Call MonthBox constructor");

        assert(parent);
        parent->addWidget(this);
        grid.x        = dayWidth;
        grid.y        = dayHeight;

        uint32_t firstDayOffset    = model->getFirstWeekOffset();
        uint32_t lastDay           = model->getLastDay();
        uint32_t iterations        = style::window::calendar::week_days_number + firstDayOffset + lastDay - 1;
        LOG_DEBUG("OFFSET DAY: %u", firstDayOffset);
        LOG_DEBUG("LAST DAY: %u", lastDay);
        uint32_t i;
        for (i = 0; i < iterations; ++i) {
            auto day = new DayLabel(app,
                                    this,
                                    i,
                                    firstDayOffset,
                                    style::window::calendar::day_cell_width,
                                    style::window::calendar::day_cell_height);
            addWidget(day);
        }

        LOG_DEBUG("MonthBox constructor Completed Successfully!");
    }

    CalendarMainWindow::CalendarMainWindow(app::Application *app, std::string name) : AppWindow(app, name)
    {
        this->actualDate = year_month_day{floor<days>(std::chrono::system_clock::now())};
        buildInterface(style::window::calendar::test::month_id);
    }

    void CalendarMainWindow::refresh()
    {
        erase(dateLabel);
        month->erase();

        monthModel = std::make_unique<MonthModel>(actualDate);
        std::string dateText = monthModel->getMonthYearText();
        this->buildMonth(monthModel);
        this->buildDateLabel(dateText);
    }

    void CalendarMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface(style::window::calendar::test::month_id);
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
        month = new MonthBox(app, this, offsetFromTop, monthWidth, monthHeight, dayWidth, dayHeight, monthModel);
        // setup month box
        addWidget(month);

            month->borderCallback = [this](const InputEvent &inputEvent) -> bool {
                if (inputEvent.state != InputEvent::State::keyReleasedShort) {
                    return false;
                }
                switch (inputEvent.keyCode) {
                case KeyCode::KEY_UP: {
                    LOG_DEBUG("change month prev");
                    if (actualDate.month() != date::month(1)) {
                        date::month prevMonth = --actualDate.month();
                        actualDate            = actualDate.year() / prevMonth / last;
                    }
                    else {
                        date::month prevMonth = date::month(12);
                        date::year prevYear   = --actualDate.year();
                        actualDate            = prevYear / prevMonth / last;
                    }
                    this->refresh();
                    return true;
                }
                case KeyCode::KEY_DOWN: {
                    LOG_DEBUG("change month next");
                    if (actualDate.month() != date::month(12)) {
                        date::month nextMonth = ++actualDate.month();
                        actualDate            = actualDate.year() / nextMonth / 1;
                    }
                    else {
                        date::month nextMonth = date::month(1);
                        date::year nextYear   = ++actualDate.year();
                        actualDate            = nextYear / nextMonth / 1;
                    }
                    this->refresh();
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

        monthModel = std::make_unique<MonthModel>(actualDate);
        this->buildMonth(monthModel);
        this->buildDateLabel(monthModel->getMonthYearText());

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
