#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include <gui/widgets/GridLayout.hpp>
#include <gui/widgets/Item.hpp>
#include <Text.hpp>
#include <map>
#include <vector>
#include <string>
#include "../ApplicationCalendar.hpp"
#include "../models/MonthModel.hpp"
#include "../models/DayModel.hpp"

namespace gui
{
    class CalendarMainWindow;

    namespace coords
    {
        const uint8_t first_row = 1;
        const uint8_t default_last_row = 5;

        const uint8_t first_column = 0;
        const uint8_t last_column = 6;
    };


    class DayLabel : public Label, public DayModel
    {  
        public:
        DayLabel(gui::Item *parent,const char *number,const uint32_t &x,const uint32_t &y, const uint32_t &width, const uint32_t &height);
        
        void setLabel(gui::Item *parent,const char *number,std::function<bool(Item &)> activatedCallback);

        void displayText()
        {
          std::string s = this->text;
          const char *c = s.c_str();
          int disp = atoi(c);
          LOG_DEBUG("%d",disp);
        }

        ~DayLabel() override = default;

    };

    class MonthBox : public GridLayout, public MonthModel
    {
      /// @param columns: defines number of columns, @param rows: defines number of rows      
      uint32_t firstRow = 0;
      uint32_t firstColumn = 0;
      uint32_t columns = 0;
      uint32_t rows = 0;
      std::map<std::pair<uint32_t,uint32_t>, DayLabel*> dayMap;

      public:
      MonthBox(app::Application *app, gui::Item *parent, const int &offsetTop, const uint32_t &width, const uint32_t &height, 
              const uint32_t &dayWidth, const uint32_t &dayHeight, const uint32_t &rows, const uint32_t &columns, const std::unique_ptr<MonthModel> &model);

      ~MonthBox() override = default;
      
      void buildMap(app::Application *app);
      std::pair<uint32_t, uint32_t> getFirstDay();
      std::pair<uint32_t, uint32_t> getLastDay();

      void setNavigation() override;
      void changeMonthInput(gui::CalendarMainWindow *parent, const uint32_t &x, const uint32_t &y, NavigationDirection direction);
      void rebuildNavigation(gui::CalendarMainWindow *parent,  const uint32_t &x,  NavigationDirection direction);
      
      uint32_t getFirstRow()
      {
        return this->firstRow;
      }

      uint32_t getFirstColumn()
      {
        return this->firstColumn;
      }

      uint32_t getColumns()
      {
        return this->columns;
      }
        
    };

    class CalendarMainWindow : public gui::AppWindow
    {
      uint32_t offsetFromTop = 0;
      uint32_t monthWidth = 0;
      uint32_t monthHeight = 0; 
      uint32_t dayWidth = 0;
      uint32_t dayHeight = 0;
      

      protected:
        MonthBox *month = nullptr;
        std::unique_ptr<MonthModel> monthModel;

      public:
        CalendarMainWindow(app::Application *app, std::string name);
        
        /// @param date_time is a value key to DB get request 
        bool getData(const uint32_t &date_time);
        
        ~CalendarMainWindow() override = default;
        void rebuild(const uint32_t &ID);
        void buildInterface(const uint32_t &actualDateTimeID);
        void destroyInterface() override;

    };

} /* namespace app */
