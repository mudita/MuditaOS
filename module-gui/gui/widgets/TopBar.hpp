// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Image.hpp"
#include "Label.hpp"
#include "Rect.hpp"
#include "TopBar/SIM.hpp"
#include <common_data/EventStore.hpp>

#include <vector>
#include <map>

namespace gui::top_bar
{
    enum class Indicator
    {
        Signal,
        Time,
        Lock,
        Battery,
        SimCard,
        NetworkAccessTechnology
    };
    using Indicators        = std::vector<Indicator>;
    using IndicatorStatuses = std::map<Indicator, bool>;

    /**
     * Carries the top bar configuration.
     */
    class Configuration
    {
      public:
        void enable(Indicator indicator);
        void enable(const Indicators &indicators);
        void disable(Indicator indicator);
        void set(Indicator indicator, bool enabled);
        [[nodiscard]] auto isEnabled(Indicator indicator) const -> bool;
        [[nodiscard]] auto getIndicatorsConfiguration() const noexcept -> const IndicatorStatuses &;

      private:
        IndicatorStatuses indicatorStatuses = {{Indicator::Signal, false},
                                               {Indicator::Time, false},
                                               {Indicator::Lock, false},
                                               {Indicator::Battery, false},
                                               {Indicator::SimCard, false},
                                               {Indicator::NetworkAccessTechnology, false}};
    };

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
        static constexpr uint32_t batteryBarsCount = 6;
        static constexpr uint32_t signalImgCount   = 6;

      public:
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
        Configuration configuration;
        static TimeMode timeMode;

        void prepareWidget();

        /// show bars in number - 0 bars, 1 bar, 2 bars...
        void batteryShowBars(uint32_t val);
        void showBattery(bool shown);
        void showSim(bool enabled);

        static uint32_t calculateBatteryBars(uint32_t percentage);

        /**
         * Sets the status of the top bar indicator.
         * @param indicator     Indicator
         */
        void setIndicatorStatus(Indicator indicator, bool enabled);

      public:
        TopBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

        /**
         * Configures the top bar.
         * @param configuration     Top bar configuration
         */
        void configure(Configuration &&config);
        [[nodiscard]] auto getConfiguration() const noexcept -> const Configuration &;

        /**
         * @brief Sets charge level of the battery based on percent value. This will cause appropriate image to be
         * displayed.
         * @return if display should be refreshed or not
         */
        bool updateBattery(uint32_t percent);
        bool updateBattery(bool plugged);

        /**
         * @brief updates signal strength. This will cause appropriate image to be displayed.
         */
        bool updateSignalStrength();

        bool updateNetworkAccessTechnology();

        void simSet();

        void setTime(const UTF8 &value);
        void setTime(uint32_t value, bool mode24H);

        UTF8 getTimeString();
        uint32_t getTime() const noexcept;

        void accept(GuiVisitor &visitor) override;
    };

} // namespace gui::top_bar
