/*
 * @file CalendarMainWindow.cpp
 * @author Tomasz Rogala (tomasz.rogala@indoornavi.me)
 * @date 6 lip 2020
 */

#include "gui/widgets/Window.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "CalendarMainWindow.hpp"
#include <gui/widgets/GridLayout.hpp>
#include <vector>
#include <string>

#include <time/time_conversion.hpp>
#include <gui/widgets/Margins.hpp>
#include <gui/widgets/Item.hpp>
#include <Text.hpp>
#include "../Common.hpp"

#include "../ApplicationCalendar.hpp"
#include "../models/DayModel.hpp"
#include "../models/DayModel.cpp"
#include "../models/MonthModel.hpp"
#include "../models/MonthModel.cpp"

namespace gui
{
    DayLabel::DayLabel(gui::Item *parent,const char *number,const uint32_t &x,const uint32_t &y, const uint32_t &width, const uint32_t &height) : Label(parent,0,0,0,0,""), DayModel(number,x,y)
    {
        LOG_DEBUG("Call DayLabel constructor. With coords: x:%d, y:%d", x,y);
        parent->addWidget(this);
        this->setSize(width, height);

        if (y == 0){
            this->setText(number);
            this->setFont(style::window::font::verysmall);  
        }
        this->activeItem = false;
        this->setPenWidth(style::window::default_border_no_focus_w);
        this->setAlignment(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
    }

    void DayLabel::setLabel(gui::Item *parent,const char *number, std::function<bool(Item &)> activatedCallback)
    {
        LOG_DEBUG("Set callback of day label. coords: x:%d, y:%d", x,y);
        this->number = number;
        this->activeItem = true;
        this->setText(number);
        this->setFont(style::window::font::medium); 
        this->activatedCallback = activatedCallback;
        this->setPenWidth(style::window::default_border_no_focus_w);
        this->setPenFocusWidth(style::window::default_border_focus_w);
        this->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_TOP | RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        this->setFont(style::window::font::medium);
    }

    MonthBox::MonthBox(app::Application *app, gui::Item *parent, const int &offsetTop, const uint32_t &width, const uint32_t &height, 
                    const uint32_t &dayWidth, const uint32_t &dayHeight, const uint32_t &rows, const uint32_t &columns,const std::unique_ptr<MonthModel> &model)
        : GridLayout(parent,style::window::default_left_margin,offsetTop,width,height,{dayWidth,dayHeight}), MonthModel(model->name.c_str(), model->days)
        {   
            LOG_DEBUG("Call MonthBox constructor");
            //set the costant values of first and first column
            this->firstRow = 0;
            this->firstColumn = 0;

            const char *week[] = {"MO","TU","WE","TH","FR","SA","SU"};
            if (parent) {
                parent->addWidget(this);
            }
            grid.x = dayWidth;
            grid.y = dayHeight;
            this->rows = rows;
            this->columns = columns;
            for (uint32_t y = 0; y < rows; y++){
                for (uint32_t x = 0; x < columns; x++){
                    auto key = std::make_pair(x,y);
                    const char *name = "";
                    if (y == 0){
                        name = week[x];
                    }
                    dayMap[key] = new DayLabel(this,name,x,y, style::window::calendar::day_cell_width,style::window::calendar::day_cell_height);
                    addWidget(dayMap[key]);
                }
            }
            LOG_DEBUG("MonthBox constructor Completed Successfully!");
            
        }

    void MonthBox::buildMap(app::Application *app)
    {
        LOG_DEBUG("Start build month box map");
        for (auto &day : days) {
            if (day->x < this->columns && day->y < this->rows){
                LOG_DEBUG("Set element in monthbox map in position x:%d y:%d",day->x,day->y);
                auto key = std::make_pair(day->x,day->y);
                dayMap[key]->displayText();
                dayMap[key]->setLabel(this, day->number.c_str(),
                    [=](gui::Item &item) {
                                {
                                    LOG_DEBUG("Switch to DayWindow");
                                    app->switchWindow("DayWindow",nullptr);
                                    return true;
                                }
                            }
                );
            }
            else{
                LOG_DEBUG("COLUMNS:%d ROWS:%d",this->columns,this->rows); 
                LOG_ERROR("Element positioned outside the box. coords x:%d , y:%d", day->x,day->y); 
            }
        }
        LOG_DEBUG("Build Map Completed Successfully!");
    }

    std::pair<uint32_t, uint32_t> MonthBox::getFirstDay()
    {
        uint32_t y = firstRow+1;
        uint32_t x = firstColumn;
        auto key = std::make_pair(x,y);
        if(!dayMap.empty()){
            while (!dayMap[key]->activeItem){
                key = std::make_pair(++x, y);
            }
        }
        return key;
    }
    std::pair<uint32_t, uint32_t> MonthBox::getLastDay()
    {
        uint32_t y = rows-2;
        uint32_t x = columns-1;
        LOG_DEBUG("KEY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! x:%d y:%d", x,y);
        auto key = std::make_pair(x,y);
        if(!dayMap.empty()){
            while ( dayMap.count(key) == 0 || !dayMap[key]->activeItem){
                LOG_DEBUG("KEY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! x:%d y:%d", x,y);
                key = std::make_pair(--x, y);
            }
        }
        LOG_DEBUG("KEY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! x:%d y:%d", x,y);
        return key;
    }

    
    void MonthBox::setNavigation()
    {  
        LOG_DEBUG("Start set navigation");
        for (uint32_t y = this->firstRow; y< this->rows; y++){
            for (uint32_t x = this->firstColumn; x< this->columns; x++){    
                    auto keyCenter = std::make_pair(x,y);
                    if (dayMap[keyCenter]->activeItem){ 

                        LOG_DEBUG("Set navigation UP from coords x:%d , y:%d", x,y);
                        auto keyUp = std::make_pair(x,y-1);
                        if(dayMap.count(keyUp) == 0 || !dayMap[keyUp]->activeItem){
                            uint32_t index = this->rows;
                            while(!dayMap[keyUp]->activeItem){
                                keyUp = std::make_pair(x,--index);
                            }
                        }
                        dayMap[keyCenter]->setNavigationItem(NavigationDirection::UP, dayMap[keyUp]);
                    
                        LOG_DEBUG("Set navigation DOWN from coords x:%d , y:%d", x,y);
                        auto keyDown = std::make_pair(x,y+1);
                        if(dayMap.count(keyDown) == 0 || !dayMap[keyDown]->activeItem){
                            uint32_t index = this->firstRow;
                            //keyDown = std::make_pair(x,index);
                            while(dayMap.count(keyDown) == 0 || !dayMap[keyDown]->activeItem){
                                keyDown = std::make_pair(x,++index);
                            }
                        }
                        dayMap[keyCenter]->setNavigationItem(NavigationDirection::DOWN, dayMap[keyDown]);

                        LOG_DEBUG("Set navigation LEFT from coords x:%d , y:%d", x,y);
                        auto keyLeft = std::make_pair(x-1,y);
                        if (dayMap.count(keyLeft) == 0 || !dayMap[keyLeft]->activeItem){
                            uint32_t index = this->columns;
                            while (dayMap.count(keyLeft) == 0 || !dayMap[keyLeft]->activeItem){
                                keyLeft = std::make_pair(--index,y);
                            }
                        }
                        dayMap[keyCenter]->setNavigationItem(NavigationDirection::LEFT, dayMap[keyLeft]);
                
                        LOG_DEBUG("Set navigation RIGHT from coords x:%d , y:%d", x,y); 
                        auto keyRight = std::make_pair(x+1,y);
                        if (dayMap.count(keyRight) == 0 || !dayMap[keyRight]->activeItem){
                            uint32_t index = this->firstColumn-1;
                            while (dayMap.count(keyRight) == 0 || !dayMap[keyRight]->activeItem){
                                keyRight = std::make_pair(++index,y);
                            }
                        }
                        dayMap[keyCenter]->setNavigationItem(NavigationDirection::RIGHT, dayMap[keyRight]);
                }
            }
        }
        LOG_DEBUG("Setting navi completed successfully");
    }

    void MonthBox::changeMonthInput(gui::CalendarMainWindow *parent, const uint32_t &x, const uint32_t &y, NavigationDirection direction)
    {
        auto key = std::make_pair(x,y);
        dayMap[key]->clearNavigationItem(direction);
        if (direction == NavigationDirection::LEFT){
            dayMap[key]->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {     
                if (inputEvent.state == InputEvent::State::keyReleasedShort) {
                        if(inputEvent.keyCode == gui::KeyCode::KEY_LEFT){
                            LOG_DEBUG("Change to prev month");
                            parent->rebuild(1);
                            return true;
                        }
                        else{
                            LOG_DEBUG("Change month not handled. Pressed key: %d", static_cast<int>(inputEvent.keyCode));
                            return false;
                        }
                }
                else {
                    LOG_DEBUG("Change month not handled. Key pressed long");
                    return false;
                }   
            };  
        }
        else if (direction == NavigationDirection::RIGHT){
                    dayMap[key]->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {     
                        if (inputEvent.state == InputEvent::State::keyReleasedShort) {
                                if(inputEvent.keyCode == gui::KeyCode::KEY_RIGHT){
                                    LOG_DEBUG("Change to next month");
                                    parent->rebuild(3);
                                    return true;
                                }    
                                else{
                                    LOG_DEBUG("Change month not handled. Pressed key: %d", static_cast<int>(inputEvent.keyCode));
                                    return false;
                                }
                        }
                        else {
                            LOG_DEBUG("Change month not handled. Key pressed long");
                            return false;
                        }   
                    };  
        }   
    }

    void MonthBox::rebuildNavigation(gui::CalendarMainWindow *parent, const uint32_t &x, NavigationDirection direction)
    {     
        //start from first row  
        uint32_t y =0;
        LOG_DEBUG("Start Rebuild navigation on coords x:%d , y:%d", x,y); 
        auto key = std::make_pair(x, y);
        while (dayMap.count(key)>0) {
            if (dayMap[key]->activeItem){
                LOG_DEBUG("Rebuild navigation on coords x:%d , y:%d", x,y);
                changeMonthInput(parent, key.first,key.second, direction);
            }
            key = std::make_pair(x, ++y);
        }
    }

    bool CalendarMainWindow::getData(const uint32_t &date_time)
    {
        switch(date_time){
        case 1:{
            //Test case 1:
            std::vector <std::unique_ptr<DayModel> > days;
            uint32_t numbOfWeeks = 4; 
            uint32_t daysInWeek = 7;
            uint32_t startDay = 0;
            uint32_t numb = 0;
            for (uint32_t y = 1; y<=numbOfWeeks; y++){
                for (uint32_t x = 0; x<daysInWeek; x++){
                    if (y == 1 && x< startDay){
                        continue;
                    }
                    ++numb;
                    if (numb>31){
                        break;
                    }
                    days.push_back(std::make_unique<DayModel>(to_string(numb).c_str(),x,y));
                }
            }
            this->monthModel = std::make_unique<MonthModel>("July",days);
            return true;
        }
        case 2:{
            //Test case 2:
            std::vector <std::unique_ptr<DayModel> > days;
            uint32_t numbOfWeeks = 5; 
            uint32_t daysInWeek = 7;
            uint32_t startDay = 3;
            uint32_t numb = 0;
            for (uint32_t y = 1; y<=numbOfWeeks; y++){
                for (uint32_t x = 0; x<daysInWeek; x++){
                    if (y == 1 && x< startDay){
                        continue;
                    }
                    ++numb;
                    if (numb>28){
                        break;
                    }
                    days.push_back(std::make_unique<DayModel>(to_string(numb).c_str(),x,y));
                }
            }
            this->monthModel = std::make_unique<MonthModel>("July",days);
            return true;
        }
        case 3:{
            //Test case 3:
            std::vector <std::unique_ptr<DayModel> > days;
            uint32_t numbOfWeeks = 5; 
            uint32_t daysInWeek = 7;
            uint32_t startDay = 6;
            uint32_t numb = 0;
            for (uint32_t y = 1; y<=numbOfWeeks; y++){
                for (uint32_t x = 0; x<daysInWeek; x++){
                    if (y == 1 && x< startDay){
                        continue;
                    }
                    ++numb;
                    if (numb>31){
                        break;
                    }
                    days.push_back(std::make_unique<DayModel>(to_string(numb).c_str(),x,y));
                }
            }
            this->monthModel = std::make_unique<MonthModel>("April",days);
            return true;
        }
        }
        return false;
    }
    
    CalendarMainWindow::CalendarMainWindow(app::Application *app, std::string name) : AppWindow(app, name)
    {
        uint32_t actualDateTime = 2;
        buildInterface(actualDateTime);
    }

    void CalendarMainWindow::rebuild(const uint32_t &ID)
    {
        destroyInterface();
        LOG_DEBUG("Calendar Date Time ID:%d",ID); 
        buildInterface(ID);
    }

    void CalendarMainWindow::buildInterface(const uint32_t &actualDateTimeID)
    {
        /// TODO:Complete Case for 7 rows
        /// TODO:Complete change month!
        LOG_DEBUG("AppWindow build interface");
        LOG_DEBUG("Calendar Date Time ID:%d",actualDateTimeID); 
        AppWindow::buildInterface();
        LOG_DEBUG("Start build interface for calendar main window");
        auto app = dynamic_cast<app::ApplicationCalendar *>(application);
        assert(app);

        setTitle("Calendar");
        offsetFromTop = title->offset_h()+style::window::calendar::month_year_height;
        monthWidth = style::window::default_body_width;
        monthHeight = style::window_height - title->offset_h() - style::footer::height; 
        dayWidth = style::window::calendar::day_cell_width;
        dayHeight = style::window::calendar::day_cell_height;
        
        bool resp = getData(actualDateTimeID);
        LOG_DEBUG("Calendar Date Time ID:%d",actualDateTimeID); 
        if(resp){
            uint32_t numbOfColums = 7;
            uint32_t numbOfRows = 8;
            //create empty month box
            month = new MonthBox(app, this, offsetFromTop, monthWidth , monthHeight,dayWidth, dayHeight,numbOfRows,numbOfColums, monthModel);
            //setup month box
            month->buildMap(app); 
            addWidget(month);
            setFocusItem(month);
            //rebuild first column to make the change to prev month available
            month->rebuildNavigation(this, month->getFirstColumn(), NavigationDirection::LEFT);
            //rebuild last column to make the change to next month available
            month->rebuildNavigation(this, (month->getColumns()-1), NavigationDirection::RIGHT);
            //rebuild first day to make the change to next month available
            auto key = month->getFirstDay();
            month->changeMonthInput(this, key.first, key.second, NavigationDirection::LEFT);
            // //rebuild last day to make the change to next month available
            // key = month->getLastDay();
            // month->changeMonthInput(this, key.first, key.second, NavigationDirection::RIGHT);
            
        }

        std::string actualDateTime = "July 2020";
        auto label = new Label(this,style::window::default_left_margin,title->offset_h(),style::window::default_body_width,style::window::calendar::month_year_height,actualDateTime);
        label->setPenWidth(style::window::default_border_no_focus_w);
        label->setFont(style::window::font::mediumbold);
        label->setAlignment(gui::Alignment(gui::Alignment::ALIGN_VERTICAL_CENTER));
        addWidget(label);

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get(style::strings::common::call));   
        LOG_DEBUG("Calendar Date Time ID:%d",actualDateTimeID); 
    }    

    void CalendarMainWindow::destroyInterface()
    {
        erase();
    }

 } /* namespace app */
