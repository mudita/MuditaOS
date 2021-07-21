// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

#include <time/time_locale.hpp>

namespace gui
{
    class DateAndTimeMainWindow : public BaseSettingsWindow
    {
      public:
        DateAndTimeMainWindow(app::Application *app, std::string name);

      protected:
        auto buildOptionsList() -> std::list<Option> override;
        virtual bool bottomBarCallback(Item &item);

        bool automaticDateAndTimeIsOn = false;

        utils::time::Locale::TimeFormat timeFormat = utils::time::Locale::defaultTimeFormat;
        utils::time::Locale::DateFormat dateFormat = utils::time::Locale::defaultDateFormat;

        std::string changeDateAndTimeWindow;
    };
} // namespace gui
