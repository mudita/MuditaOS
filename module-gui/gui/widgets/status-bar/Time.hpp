// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "StatusBarWidgetBase.hpp"
#include <time/time_conversion.hpp>
#include <Label.hpp>

namespace gui::status_bar
{

    /// Sets time mode (12h/24h) for Time widget
    class TimeConfiguration : public StatusBarVisitor
    {
      public:
      
        enum class TimeMode
        {
            Time12h, /// 12h time format
            Time24h  /// 24h time format
        };

        explicit TimeConfiguration(TimeMode mode);

        [[nodiscard]] TimeMode getMode() const noexcept;
        void visit(gui::status_bar::Time &widget) const override;

      private:
        TimeMode mode;
    };

    class Time : public StatusBarWidgetBase<Label>
    {
        utils::time::Timestamp _time;
        void setFormat(const std::string &format);
        friend class TimeConfiguration;

      public:
        Time(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        void update();
        void acceptStatusBarVisitor(StatusBarVisitor &visitor) override;
    };
} // namespace gui::status_bar
