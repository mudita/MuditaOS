/*
 * @file TopBar.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 22 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_GUI_GUI_WIDGETS_TOPBAR_HPP_
#define MODULE_GUI_GUI_WIDGETS_TOPBAR_HPP_

#include "Image.hpp"
#include "Label.hpp"
#include "Rect.hpp"
#include "TopBar/SIM.hpp"
#include <common_data/EventStore.hpp>

namespace gui {

    static const uint32_t batteryLevelCount = 6;
    static const uint32_t signalImgCount = 6;

    /// Header of most of design Windows
    ///
    /// Provides way to show:
    ///    0. Window title
    ///    1. battery state
    ///    2. signal
    ///    3. time
    ///    4. sim state
    /// in separate areas, when enabled
    ///
    ///              [time]
    /// [signal]    [title ] [sim] [battery]
    class TopBar : public Rect
    {
        static const uint32_t signalOffset;
        static const uint32_t batteryOffset;

      public:
        enum class Elements
        {
            SIGNAL = 0x01,
            LOCK,
            BATTERY,
            TIME,
            SIM,
        };
        enum class TimeMode
        {
            TIME_12H,
            TIME_24H
        };
        static uint32_t time;

      protected:
        static Store::SignalStrength::RssiBar signalStrengthBar;
        Label *timeLabel;
        Image *signal[6];
        Image *lock;
        std::array<Image *, batteryLevelCount> battery = {nullptr};
        Label *charging = nullptr;
        gui::SIM *sim = nullptr;
        void prepareWidget();
        static TimeMode timeMode;

        /// show bars in number - 0 bars, 1 bar, 2 bars...
        void batteryShowBars(uint32_t val);

        /// elements shown on TopBar
        struct
        {
            bool signal : 1;
            bool lock : 1;
            bool battery : 1;
            bool time : 1;
            bool sim : 1;
        } elements = {false, false, false, false, true};

      public:
        TopBar();
        TopBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        virtual ~TopBar();

        /**
         * @brief Sets mode of time displaying according to TimeMode enum
         * @note Variable is common for all instances of TopBar
         */

        /**
         * @brief Hides or shows images.
         * @note LOCK and TIME are located in the same place so only 1 can be active at the same time.
         */
        void setActive(TopBar::Elements element, bool active);
        void setActive(std::list<std::pair<TopBar::Elements, bool>> elements);
        /**
         * @brief Sets charge level of the battery based on percent value. This will cause appropriate image to be displayed.
         * @return if display should be refreshed or not
         */
        bool setBatteryLevel(uint32_t percent);

        void setBatteryCharging(bool plugged);
        /**
         * @brief Sets signal strength. This will cause appropriate image to be displayed.
         */
        void setSignalStrength(Store::SignalStrength::RssiBar sth);
        /**
         * @brief updates signal strength. This will cause appropriate image to be displayed.
         */
        void updateSignalStrength();

        void simSet();
        void setTime(const UTF8 &time);
        void setTime(const uint32_t &time, bool mode24H);
        UTF8 getTimeString();
        uint32_t getTime()
        {
            return time;
        };

        // virtual methods from Item
        std::list<DrawCommand *> buildDrawList() override;
};

} /* namespace gui */

#endif /* MODULE_GUI_GUI_WIDGETS_TOPBAR_HPP_ */
