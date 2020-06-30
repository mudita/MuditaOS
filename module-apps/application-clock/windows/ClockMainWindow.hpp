#pragma once

// module-gui
#include "AppWindow.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Progress.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "gui/widgets/Progress.hpp"

namespace gui
{

    class ClockMainWindow : public AppWindow
    {
        uint32_t seconds      = 0;
        uint32_t hour         = 0;
        uint32_t minute       = 0;
        Label *hourLabel      = nullptr;
        Label *minuteLabel    = nullptr;
        Progress *progressBar = nullptr;

      public:
        ClockMainWindow(app::Application *app);

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
