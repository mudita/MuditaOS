#pragma once
#include "Application.hpp"
#include <gui/widgets/GridLayout.hpp>
#include "application-calendar/models/MonthModel.hpp"

namespace gui
{
    class MonthBox : public GridLayout
    {
      public:
        MonthBox(app::Application *app,
                 gui::Item *parent,
                 const int &offsetTop,
                 const uint32_t &width,
                 const uint32_t &height,
                 const uint32_t &dayWidth,
                 const uint32_t &dayHeight,
                 const std::unique_ptr<MonthModel> &model,
                 bool *isDayEmpty);

        ~MonthBox() override = default;
        std::string month;
        date::year_month monthFilterValue;
    };

} /* namespace gui */
