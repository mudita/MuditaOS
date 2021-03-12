// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include "time/time_locale.hpp"

namespace gui
{
    class DateAndTimeMainWindow : public BaseSettingsWindow
    {
      public:
        explicit DateAndTimeMainWindow(app::Application *app);

      protected:
        auto buildOptionsList() -> std::list<Option> override;
        virtual bool bottomBarCallback(Item &item);

        bool automaticDateAndTimeIsOn = false;
        bool automaticTimeZoneIsOn    = false;

        utils::time::Locale::TimeFormat timeFormat = utils::time::Locale::defaultTimeFormat;
        utils::time::Locale::DateFormat dateFormat = utils::time::Locale::defaultDateFormat;
    };
} // namespace gui
