#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/GridLayout.hpp>
#include <gui/widgets/Item.hpp>
#include <Text.hpp>
#include <map>
#include <vector>

namespace gui
{

    namespace coords
    {
        const uint8_t first_row = 1;
        const uint8_t default_last_row = 5;

        const uint8_t first_column = 0;
        const uint8_t last_column = 6;
    };

    class Day : public Label
    {  
        public:
        uint8_t number;

        public:
        Day(const uint8_t &number, const uint32_t &c_width, const uint32_t &c_height, std::function<bool(Item &)> activatedCallback, const bool &week); 
    };

    class Year : public Label
    {
      protected: 
        bool leap;

        public:
        Year(gui::Item *parent,std::string text, const uint32_t &x, const uint32_t &y, const uint32_t &width, const uint32_t &height);
    };

    class Month : public gui::GridLayout
    {
      protected:
      ///daymap first value (std::pair) represents coordinates to exact position of current (Day) element in month grid 
      ///Maybe the map container for grid elements with coordinate keys should be implemented directly in gridLayout class
      ///Get element through coords makes using GRID much simpler
      std::map<std::pair<uint32_t,uint32_t>, Day*> dayMap;

      // temporary solution
      //days will be filled with day numbers in current month 
      std::vector<uint8_t> days;

      ///define how many days will be displayed in february
      bool year_leap = true;

      public:
      ///example value:
      uint32_t numberOfDays = 30;

      Month(app::Application *app,gui::Item *parent, std::string name, const int &title_offset,const uint32_t &width, const uint32_t &height,
            const uint32_t &dayWidth, const uint32_t &dayHeight);
      virtual ~Month() = default;

      void createDayMap(app::Application *app);
      void clearDayMap();
      
      /// @param days is filled with current month structure 
      // temporary solution
      void get_month(const uint32_t &offset);

      void rebuildNavigation();

      uint32_t getFirstDayCoord_X();
      uint32_t getFirstDayCoord_Y();

      uint32_t getLastDayCoord_X();
      uint32_t getLastDayCoord_Y();
    };


    class CalendarMainWindow : public gui::AppWindow
    {
      protected:
        ///gridLayout container
        ///Displays all days within current month. 
        Month *month = nullptr;
        ///Label container
        ///Displays year and month value
        Year *year = nullptr;

      public:
        CalendarMainWindow(app::Application *app, std::string name);
        
        ///Assumed that all memory allocations are cleared within base class destructors
        virtual ~CalendarMainWindow() = default;
        
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

    };

} /* namespace app */
