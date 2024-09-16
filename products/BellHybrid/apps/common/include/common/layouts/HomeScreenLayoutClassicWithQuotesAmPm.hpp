// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "HomeScreenLayoutClassicWithQuotes.hpp"

namespace gui
{
    class HomeScreenLayoutClassicWithQuotesAmPm : public HomeScreenLayoutClassicWithQuotes
    {
      public:
        explicit HomeScreenLayoutClassicWithQuotesAmPm(std::string name);

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;

      protected:
        void buildInterface() override;

        TextFixedSize *ampm{nullptr};
    };
} // namespace gui
