// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_GUI_GUI_WIDGETS_TOPBAR_HPP_
#define MODULE_GUI_GUI_WIDGETS_TOPBAR_HPP_

#include "Image.hpp"
#include "Label.hpp"
#include "Rect.hpp"
#include "TopBar/SIM.hpp"
#include <common_data/EventStore.hpp>
#include <map>

namespace gui
{

    static const uint32_t batteryBarsCount  = 6;
    static const uint32_t signalImgCount    = 6;

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
            NETWORK_ACCESS_TECHNOLOGY
        };
        enum class TimeMode
        {
            TIME_12H,
            TIME_24H
        };
        static uint32_t time;

      protected:
        Label *timeLabel                    = nullptr;
        Label *networkAccessTechnologyLabel = nullptr;
        Image *signal[static_cast<size_t>(Store::RssiBar::noOfSupprtedBars)];
        Image *lock;
        std::array<Image *, batteryBarsCount> batteryBars               = {nullptr};
        std::map<const Store::Battery::State, Image *> batteryChargings = {
            {Store::Battery::State::Charging, nullptr}, {Store::Battery::State::PluggedNotCharging, nullptr}};
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
            bool networkAccessTechnology : 1;
        } elements = {false, false, false, false, true, true};

      public:
        TopBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

        /**
         * @brief Hides or shows images.
         * @note LOCK and TIME are located in the same place so only 1 can be active at the same time.
         */
        void setActive(TopBar::Elements element, bool active);
        void setActive(std::list<std::pair<TopBar::Elements, bool>> elements);
        /**
         * @brief Sets charge level of the battery based on percent value. This will cause appropriate image to be
         * displayed.
         * @return if display should be refreshed or not
         */
        bool updateBattery(uint32_t percent);
        bool updateBattery(bool plugged);
        void showBattery(bool shown);

        /**
         * @brief updates signal strength. This will cause appropriate image to be displayed.
         */
        bool updateSignalStrength();

        bool updateNetworkAccessTechnology();

        void simSet();
        void setTime(const UTF8 &time);
        void setTime(const uint32_t &time, bool mode24H);
        UTF8 getTimeString();
        uint32_t getTime()
        {
            return time;
        };
        void accept(GuiVisitor &visitor) override;
    };

} /* namespace gui */

#endif /* MODULE_GUI_GUI_WIDGETS_TOPBAR_HPP_ */
