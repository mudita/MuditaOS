// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "HomeScreenLayoutClassic.hpp"

namespace gui
{
    class HomeScreenLayoutClassicWithQuotes : public HomeScreenLayoutClassic
    {
      public:
        HomeScreenLayoutClassicWithQuotes(std::string name);

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        void setBatteryLevelState(const Store::Battery &batteryContext) override;
        void setUSBStatusConnected() override;

      protected:
        void buildInterface() override;

        VBox *textBox{nullptr};
        TextFixedSize *quotes{nullptr};
        TextFixedSize *author{nullptr};
        TextFixedSize *ampm{nullptr};
        TextFixedSize *usbStatus{nullptr};
        VBox *imgBox{nullptr};
        Image *quoteImg{nullptr};
        bool showAMPM{false};
    };
}; // namespace gui
