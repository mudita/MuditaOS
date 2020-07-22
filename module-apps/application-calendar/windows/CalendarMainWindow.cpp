#include "CalendarMainWindow.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include "application-calendar/models/DayModel.hpp"
#include "application-calendar/models/MonthModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/models/CalendarEventsModel.hpp"
#include "application-calendar/models/CalendarAllEventsModel.hpp"
#include "NoEvents.hpp"
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

    void MonthBox::buildMap(app::Application *app)
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

    std::pair<uint32_t, uint32_t> MonthBox::getFirstDay()
    {
        uint32_t y = firstRow + 1;
        uint32_t x = firstColumn;
        auto key   = std::make_pair(x, y);
        if (!dayMap.empty()) {
            while (!dayMap[key]->activeItem) {
                key = std::make_pair(++x, y);
            }
        }
        return key;
    }
    std::pair<uint32_t, uint32_t> MonthBox::getLastDay()
    {
        uint32_t y = rows - 2;
        uint32_t x = columns - 1;
        auto key   = std::make_pair(x, y);
        if (!dayMap.empty()) {
            while (dayMap.count(key) == 0 || !dayMap[key]->activeItem) {
                key = std::make_pair(--x, y);
            }
        }
        return key;
    }

    void MonthBox::setNavigation()
    {
        LOG_DEBUG("Start setting navigation");
        // Go through each element of grid
        for (uint32_t y = this->firstRow; y < this->rows; y++) {
            for (uint32_t x = this->firstColumn; x < this->columns; x++) {
                auto keyCenter = std::make_pair(x, y);
                // if current element is active set the navigation for all neighbours in 4 directions
                if (dayMap[keyCenter]->activeItem) {
                    // direction UP
                    LOG_DEBUG("Set navigation UP from coords x:%d , y:%d", static_cast<int>(x), static_cast<int>(y));
                    auto keyUp = std::make_pair(x, y - 1);
                    // if neighbour on top is not active or outside the map it means the current element is at the
                    // top border
                    if (dayMap.count(keyUp) == 0 || !dayMap[keyUp]->activeItem) {
                        /// get the grid element from the opposite row
                        // change neighbour coords to the last row
                        uint32_t index = this->rows;
                        // go up to the first active element
                        while (!dayMap[keyUp]->activeItem) {
                            keyUp = std::make_pair(x, --index);
                        }
                    }
                    // set navigation from current element to the upper neighbour
                    dayMap[keyCenter]->setNavigationItem(NavigationDirection::UP, dayMap[keyUp]);

                    // direction DOWN
                    LOG_DEBUG("Set navigation DOWN from coords x:%d , y:%d", static_cast<int>(x), static_cast<int>(y));
                    auto keyDown = std::make_pair(x, y + 1);
                    // if bottom neighbour is not active or outside the map it means the current element is at the
                    // bottom border
                    if (dayMap.count(keyDown) == 0 || !dayMap[keyDown]->activeItem) {
                        /// get the grid element from the opposite row
                        // change neighbour coords to the first row
                        uint32_t index = this->firstRow;
                        // go down to the first active element
                        while (dayMap.count(keyDown) == 0 || !dayMap[keyDown]->activeItem) {
                            keyDown = std::make_pair(x, ++index);
                        }
                    }
                    // set navigation from current element to the bottom neighbour
                    dayMap[keyCenter]->setNavigationItem(NavigationDirection::DOWN, dayMap[keyDown]);

                    // direction LEFT
                    LOG_DEBUG("Set navigation LEFT from coords x:%d , y:%d", static_cast<int>(x), static_cast<int>(y));
                    auto keyLeft = std::make_pair(x - 1, y);
                    // if left neighbour is not active or outside the map it means the current element is at the
                    // left border
                    if (dayMap.count(keyLeft) == 0 || !dayMap[keyLeft]->activeItem) {
                        /// get the grid element from the opposite column
                        // change neighbour coords to the last column
                        uint32_t index = this->columns;
                        // go left to the first active element
                        while (dayMap.count(keyLeft) == 0 || !dayMap[keyLeft]->activeItem) {
                            keyLeft = std::make_pair(--index, y);
                        }
                    }
                    // set navigation from current element to the left neighbour
                    dayMap[keyCenter]->setNavigationItem(NavigationDirection::LEFT, dayMap[keyLeft]);

                    // direction RIGHT
                    LOG_DEBUG("Set navigation RIGHT from coords x:%d , y:%d", static_cast<int>(x), static_cast<int>(y));
                    auto keyRight = std::make_pair(x + 1, y);
                    // if right neighbour is not active or outside the map it means the current element is at the
                    // right border
                    if (dayMap.count(keyRight) == 0 || !dayMap[keyRight]->activeItem) {
                        /// get the grid element from the opposite column
                        // change neighbour coords to the first column
                        uint32_t index = this->firstColumn - 1;
                        while (dayMap.count(keyRight) == 0 || !dayMap[keyRight]->activeItem) {
                            keyRight = std::make_pair(++index, y);
                        }
                    }
                    // set navigation from current element to the right neighbour
                    dayMap[keyCenter]->setNavigationItem(NavigationDirection::RIGHT, dayMap[keyRight]);
                }
            }
        }
        LOG_DEBUG("Setting navi completed successfully");
    }

    void MonthBox::changeMonthInput(gui::CalendarMainWindow *parent,
                                    const uint32_t &x,
                                    const uint32_t &y,
                                    NavigationDirection direction)
    {
        auto key = std::make_pair(x, y);
        dayMap[key]->clearNavigationItem(direction);
        if (direction == NavigationDirection::LEFT) {
            dayMap[key]->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
                if (inputEvent.state == InputEvent::State::keyReleasedShort &&
                    inputEvent.keyCode == gui::KeyCode::KEY_LEFT) {
                    LOG_DEBUG("Change to prev month");
                    parent->rebuild(style::window::calendar::test::prev_month_id);
                    return true;
                }
                else {
                    LOG_DEBUG("Change month not handled. Pressed key: %d", static_cast<int>(inputEvent.keyCode));
                    return false;
                }
            };
        }
        else if (direction == NavigationDirection::RIGHT) {
            dayMap[key]->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
                if (inputEvent.state == InputEvent::State::keyReleasedShort &&
                    inputEvent.keyCode == gui::KeyCode::KEY_RIGHT) {
                    LOG_DEBUG("Change to next month");
                    parent->rebuild(style::window::calendar::test::next_month_id);
                    return true;
                }
                else {
                    LOG_DEBUG("Change month not handled. Pressed key: %d", static_cast<int>(inputEvent.keyCode));
                    return false;
                }
            };
        }
    }

    void MonthBox::rebuildNavigation(gui::CalendarMainWindow *parent, const uint32_t &x, NavigationDirection direction)
    {
        // start from first row
        uint32_t y = 0;
        LOG_DEBUG("Start Rebuild navigation on coords x:%d , y:%d", static_cast<int>(x), static_cast<int>(y));
        auto key = std::make_pair(x, y);
        while (dayMap.count(key) > 0) {
            if (dayMap[key]->activeItem) {
                LOG_DEBUG("Rebuild navigation on coords x:%d , y:%d", static_cast<int>(x), static_cast<int>(y));
                changeMonthInput(parent, key.first, key.second, direction);
            }
            key = std::make_pair(x, ++y);
        }
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

    void CalendarMainWindow::rebuild(const uint32_t &ID)
    {
        destroyInterface();
        LOG_DEBUG("Calendar Date Time ID:%d", static_cast<int>(ID));
        buildInterface(ID);
    }

    void CalendarMainWindow::buildInterface(const uint32_t &actualDateTimeID)
    {
        LOG_DEBUG("AppWindow build interface");
        LOG_DEBUG("Calendar Date Time ID:%d", static_cast<int>(actualDateTimeID));
        AppWindow::buildInterface();

        LOG_DEBUG("Start build interface for calendar main window");
        auto app = dynamic_cast<app::ApplicationCalendar *>(application);
        assert(app);

        setTitle(utils::localize.get("app_calendar_title_main"));
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
            setFocusItem(month);
            // rebuild first column to make the change to prev month available
            month->rebuildNavigation(this, month->getFirstColumn(), NavigationDirection::LEFT);
            // rebuild last column to make the change to next month available
            month->rebuildNavigation(this, (month->getColumns() - 1), NavigationDirection::RIGHT);
            // rebuild first day to make the change to next month available
            auto key = month->getFirstDay();
            month->changeMonthInput(this, key.first, key.second, NavigationDirection::LEFT);
        }

        auto actualDateTime = "June 2019";
        // CHange label into bar
        auto label = new Label(this,
                               style::window::default_left_margin,
                               title->offset_h(),
                               style::window::default_body_width,
                               style::window::calendar::month_year_height,
                               actualDateTime);
        // label->setActive(gui::TopBar::Elements::TIME, true);
        label->setPenWidth(style::window::default_border_no_focus_w);
        label->setFont(style::window::font::mediumbold);
        label->setAlignment(gui::Alignment(gui::Alignment::Vertical::Center));
        addWidget(label);

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
            std::shared_ptr<CalendarEventsModel> calendarEventsModel =
                std::make_shared<CalendarEventsModel>(this->application);
            if (calendarEventsModel->getItemCount() == 0) {
                switchToNoEventsWindow();
            }
            else {
                LOG_DEBUG("Switch to Day Window");
                application->switchWindow(style::window::calendar::name::day_events_window);
            }
            return true;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_LF) {
            std::shared_ptr<CalendarAllEventsModel> calendarAllEventsModel =
                std::make_shared<CalendarAllEventsModel>(this->application);
            if (calendarAllEventsModel->getItemCount() == 0) {
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
