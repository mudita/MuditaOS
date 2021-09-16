// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

// module-gui
#include "AppWindow.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "gui/widgets/ProgressBar.hpp"

namespace gui
{

    class ClockMainWindow : public AppWindow
    {
        uint32_t seconds         = 0;
        uint32_t hour            = 0;
        uint32_t minute          = 0;
        Label *hourLabel         = nullptr;
        Label *minuteLabel       = nullptr;
        ProgressBar *progressBar = nullptr;

      public:
        ClockMainWindow(app::ApplicationCommon *app, const std::string &name);

        bool handleSwitchData(SwitchData *data) override;

        void setHour(uint32_t h)
        {
            hour = h % 24;
        };
        void setMinute(uint32_t m)
        {
            minute = m % 60;
        };

        /**
         * @brief Increments hours counter
         */
        bool incrementHour();
        /**
         * @brief Increments minutes counter
         */
        bool incrementMinute();
        /**
         * @brief Increments seconds counter
         */
        bool incrementSecond();
        /**
         * @brief Updates strings for hour and minutes
         */
        void updateLabels();

        void rebuild(){};
        void buildInterface()
        {
            AppWindow::buildInterface();
        };
        void destroyInterface()
        {
            AppWindow::destroyInterface();
        };
    };

} /* namespace gui */
