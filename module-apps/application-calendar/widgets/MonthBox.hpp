// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <models/MonthModel.hpp>

#include <apps-common/ApplicationCommon.hpp>
#include <gui/widgets/GridLayout.hpp>

namespace gui
{
    class MonthBox : public GridLayout
    {
      public:
        MonthBox(app::ApplicationCommon *app,
                 gui::Item *parent,
                 const int &offsetTop,
                 const uint32_t &width,
                 const uint32_t &height,
                 const uint32_t &dayWidth,
                 const uint32_t &dayHeight,
                 const std::unique_ptr<MonthModel> &model,
                 std::array<bool, 31> &isDayEmpty);

        ~MonthBox() override = default;
        std::string month;
        date::year_month monthFilterValue;
    };

} /* namespace gui */
