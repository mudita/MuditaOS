// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "HomeScreenLayoutClassic.hpp"

namespace gui
{
    class HomeScreenLayoutClassicWithDate : public HomeScreenLayoutClassic
    {
      public:
        explicit HomeScreenLayoutClassicWithDate(std::string name);

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;

      protected:
        void buildInterface() override;
        bool isBatteryVisibilityAllowed(const Store::Battery &batteryContext) override;

        Text *date{nullptr};
        TextFixedSize *ampm{nullptr};
    };
} // namespace gui
