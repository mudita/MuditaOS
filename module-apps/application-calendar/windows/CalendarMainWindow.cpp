/*
 * @file CalendarMainWindow.cpp
 * @author Tomasz Rogala (tomasz.rogala@indoornavi.me)
 * @date 6 lip 2020
 */

#include "gui/widgets/Label.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "CalendarMainWindow.hpp"
#include <time/time_conversion.hpp>
#include <gui/widgets/Margins.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/GridLayout.cpp>
#include <gui/widgets/Item.hpp>
#include <Text.hpp>
#include "../Common.hpp"
#include "../ApplicationCalendar.hpp"
#include <vector>

namespace gui
{
    Year::Year(gui::Item *parent,std::string text, const uint32_t &x, const uint32_t &y, const uint32_t &width, const uint32_t &height)
    {
        this->setPosition(x, y);
        this->setSize(width, height);
        this->setText(text);
            
        this->setFont(style::window::font::mediumbold);
        this->setPenWidth(style::window::default_border_no_focus_w);
        this->setAlignment(gui::Alignment(gui::Alignment::ALIGN_VERTICAL_CENTER));
    }

    Day::Day(const uint8_t &number, const uint32_t &width, const uint32_t &height, std::function<bool(Item &)> activatedCallback, const bool &week)
    {
        this->setSize(width, height);
        if (week) {
            if(number < 7) {
                switch(number){
                    case 0:{ this->setText("MO"); break;}
                    case 1:{ this->setText("TU"); break;}
                    case 2:{ this->setText("WE"); break;}
                    case 3:{ this->setText("TH"); break;}
                    case 4:{ this->setText("FR"); break;}
                    case 5:{ this->setText("SA"); break;}
                    case 6:{ this->setText("SU"); break;}
                }
                this->activeItem = false;
                this->setFont(style::window::font::verysmall);
                this->setPenWidth(style::window::default_border_no_focus_w);
                this->setAlignment(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
            }
            else {
                LOG_ERROR("Wrong day number");
            } 
        }
        else {
            if (!number){
                this->setText("");
                this->activeItem = false;
                this->visible = false;
            }
            else if(number > 0 && number <= 31) {
                this->setPosition(20,20);
                this->activatedCallback = activatedCallback;
                this->setPenWidth(style::window::default_border_no_focus_w);
                this->setPenFocusWidth(style::window::default_border_focus_w);
                this->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_TOP | RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
                this->setText(to_string(number));
                this->setFont(style::window::font::medium);
                this->setAlignment(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
            }
            else {
                LOG_ERROR("Wrong day number");
            } 
        }
        if (!week)
            this->number = number;
           
    }

    Month::Month(app::Application *app, gui::Item *parent, std::string name, const int &title_offset, const uint32_t &width, const uint32_t &height, 
                const uint32_t &dayWidth, const uint32_t &dayHeight)
    : GridLayout(parent,style::window::default_left_margin,title_offset,width,height,{dayWidth,dayHeight})
    {
        clearDayMap();
        createDayMap(app);
        if (parent) {
            parent->addWidget(this);
        }
        for (int y = 0; y<7;y++)
            for (int x = 0; x<7;x++){                    
                auto key = std::make_pair(x,y);
                //addWidget secures before adding nullptr
                this->addWidget(dayMap[key]);  
            }
    }

    void Month::clearDayMap()
    {
        for (int y = 0; y<10;y++){
            for (int x = 0; x<10;x++){                    
                auto key = std::make_pair(x,y);
                dayMap[key] = nullptr; 
            }
        }
    }
    void Month::get_month(const int &offset)
    {
        /// @param offset defines number of empty fields in first row 
        /// This is a temporary solution
        for (int i = 0; i<offset; i++) {
            days.push_back(0);
        }
        for (int i = 1; i<=numberOfDays; i++) {
            days.push_back(i);
        }
        ///   day record in DB will have a coordinate value...
        ///...So days will be imported directly from database to dayMap 
    }

    void Month::createDayMap(app::Application *app)
    {
        get_month(4);
        // When DB will be created days will be directly imported from DB
        uint32_t col = 0;
        uint32_t row = 0;

        //Create labels in first row of month grid (name of week days)
        const uint8_t weekDayNumbs[] = {0,1,2,3,4,5,6};
        for (auto weekDayNumb : weekDayNumbs){
                auto pair = std::make_pair(col,row);
                dayMap[pair] = new gui::Day{
                        weekDayNumb,style::window::calendar::day_cell_width,
                        style::window::calendar::day_cell_height,
                        [=](gui::Item &item) { { return true;} },
                        true
                        };
                ++col;
        }
        col = 0; 
        ++row;
        //create all month day labels
        for (auto day : days){
                auto pair = std::make_pair(col,row);
                dayMap[pair] = new gui::Day{day,style::window::calendar::day_cell_width,style::window::calendar::day_cell_height,
                        [=](gui::Item &item) {
                            {
                                LOG_DEBUG("Switch to DayWindow");
                                /// TODO: Change DayWindow object title according to dayNumb value
                                app->switchWindow("DayWindow",nullptr);
                                return true;
                            }
                        },
                        false
                    };
                ++col;
                if (col == 7){
                    col = 0; 
                    ++row;
                }      
        }
    }

    uint32_t Month::getFirstDayCoord_X()
    {
        auto key = std::make_pair(coords::last_column, coords::first_row);
        return coords::last_column - dayMap[key]->number +1;
    }

    uint32_t Month::getFirstDayCoord_Y()
    {
        return coords::first_row;
    }

    uint32_t Month::getLastDayCoord_X()
    {   
        uint32_t lastRow = getLastDayCoord_Y();
        auto key = std::make_pair(coords::first_column,lastRow);
        return this->numberOfDays - dayMap[key]->number;
    }

    uint32_t Month::getLastDayCoord_Y()
    {
        //Calculate last row in grid
        uint32_t lastRow;
        if (dayMap[std::make_pair(coords::first_column,coords::default_last_row+1)] == nullptr)
            lastRow = coords::default_last_row;
        else
            lastRow = coords::default_last_row+1;
        return lastRow;
    }

    void Month::rebuildNavigation()
    {  /* Description:
        * Rebuilds navigation in irregular grid
        * Designed only for calendar needs
        * 
        * More simple and complex solution: 
        * -Override the gridLayout function setNavigation()
        * -Build navigation from the basics 
        */
        int lastRow = getLastDayCoord_Y();

        auto keyFirst = std::make_pair(getFirstDayCoord_X(), getFirstDayCoord_Y());
        auto keyLast = std::make_pair(getLastDayCoord_X(), getLastDayCoord_Y());

        //add navi from last day to first 
        /// TODO: Probably should be changed conception
        dayMap[keyLast]->setNavigationItem(NavigationDirection::RIGHT, dayMap[keyFirst]);
        //add navi from first day to last
        dayMap[keyFirst]->setNavigationItem(NavigationDirection::LEFT, dayMap[keyLast]);

        //rebuild navigation between last and first row (last row <-) 
        for(int x = keyLast.first; x>=coords::first_column; --x) {
            auto lastRowKey = std::make_pair(x,lastRow);
            uint32_t y = coords::first_row;
            while(!(dayMap[std::make_pair(x,y)]->activeItem)){
                ++y;
            }
            auto firstRowKey = std::make_pair(x,y);
            dayMap[lastRowKey]->setNavigationItem(NavigationDirection::DOWN, dayMap[firstRowKey]);
            dayMap[firstRowKey]->setNavigationItem(NavigationDirection::UP, dayMap[lastRowKey]);
        }

        //rebuild navigation between last and first row (last row ->)
        for(int x = keyLast.first+1; x<=coords::last_column; x++) {
            auto lastRowKey = std::make_pair(x,lastRow-1);
            uint32_t y = coords::first_row;
            while(!(dayMap[std::make_pair(x,y)]->activeItem)){
                ++y;
            }
            auto firstRowKey = std::make_pair(x,y);
            dayMap[lastRowKey]->setNavigationItem(NavigationDirection::DOWN, dayMap[firstRowKey]);
            dayMap[firstRowKey]->setNavigationItem(NavigationDirection::UP, dayMap[lastRowKey]);
        }

        
        ///=======temporary solution:
        //navi from first row, last element
        auto key = std::make_pair(coords::last_column, coords::first_row);
        dayMap[key]->setNavigationItem(NavigationDirection::RIGHT, dayMap[keyFirst]);
        ///========================
        ///-------------------------TODO:-----------------------------------------
        // Change month while navigate from last column in the direction::RIGHT
        //int y = coords::last_column;
        for(int x = coords::first_row; x<=lastRow; x++) {
            //auto key = std::make_pair(x, y);
            //Set input right to change_to next_month event
            LOG_DEBUG("TODO");
        }
        // Change month while navigate from first column in the direction::LEFT
        //y = coords::last_column;
        for(int x = coords::first_row; x<=lastRow; x++) {
            //auto key = std::make_pair(x, y);
            //Set input right to change_to next_month event
            LOG_DEBUG("TODO");
        }
        //----------------------------------------------------------------------

    }
    
    CalendarMainWindow::CalendarMainWindow(app::Application *app, std::string name) : AppWindow(app, name)
    {
        buildInterface();
    }

    void CalendarMainWindow::rebuild()
    {}

    void CalendarMainWindow::buildInterface()
    {
        AppWindow::buildInterface();
        setTitle("Calendar");
        
        auto app = dynamic_cast<app::ApplicationCalendar *>(application);
        assert(app);

        year = new Year(this, " June 2019", style::window::default_left_margin, title->offset_h(), style::window::default_body_width,style::window::calendar::month_year_height);
        this->addWidget(year);
        setFocusItem(year);

        uint32_t monthWidth = style::window::default_body_width;
        uint32_t monthHeight = style::window_height - title->offset_h() - style::footer::height; 
        uint32_t dayWidth = style::window::calendar::day_cell_width;
        uint32_t dayHeight = style::window::calendar::day_cell_height;
        uint32_t offsetFromTop = title->offset_h()+style::window::calendar::month_year_height;

        month = new Month(app, this, "June", offsetFromTop,monthWidth, monthHeight,dayWidth, dayHeight);
        setFocusItem(month);
        month->rebuildNavigation();

        // ///-------------to remove----------------------------------------------
        // auto keyFirst = std::make_pair(getFirstDayCoord_X(),getFirstDayCoord_Y());
        // auto keyLast = std::make_pair(getLastDayCoord_X(),getLastDayCoord_Y());
        // auto temp = dayMap[keyFirst]->getNavigationItem(NavigationDirection::LEFT);
        // auto temp2 = dayMap[keyLast];
        // LOG_DEBUG("COMPARE OBJECTS AFTER FOCUS:  %" PRIxPTR "\n",reinterpret_cast<std::intptr_t>(temp));
        // LOG_DEBUG("COMPARE OBJECTS AFTER FOCUS:  %" PRIxPTR "\n",reinterpret_cast<std::intptr_t>(temp2));
        // ///-------------to remove----------------------------------------------

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get(style::strings::common::list));
            
    }

    void CalendarMainWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
    }

 } /* namespace app */
