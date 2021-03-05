// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <vector>
#include <map>

namespace gui
{
    class Label;
    class Image;

    namespace top_bar
    {
        class SignalStrengthBase;
        class BatteryBase;
        class NetworkAccessTechnology;
        class SIM;
        class Time;
        class Lock;
    } // namespace top_bar
} // namespace gui

class UTF8;

namespace gui::top_bar
{
    enum class Indicator
    {
        Signal,
        Time,
        Lock,
        Battery,
        SimCard,
        NetworkAccessTechnology,
    };
    using Indicators        = std::vector<Indicator>;
    using IndicatorStatuses = std::map<Indicator, bool>;

    enum class TimeMode
    {
        Time12h,
        Time24h
    };

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
        void set(TimeMode timeMode);
        [[nodiscard]] auto getTimeMode() const noexcept -> TimeMode;
        [[nodiscard]] auto isEnabled(Indicator indicator) const -> bool;
        [[nodiscard]] auto getIndicatorsConfiguration() const noexcept -> const IndicatorStatuses &;

      private:
        IndicatorStatuses indicatorStatuses = {{Indicator::Signal, false},
                                               {Indicator::Time, false},
                                               {Indicator::Lock, false},
                                               {Indicator::Battery, false},
                                               {Indicator::SimCard, false},
                                               {Indicator::NetworkAccessTechnology, false}};
        TimeMode timeMode                   = TimeMode::Time12h;
    };

    /// Header of most of design Windows
    class TopBar : public HBox
    {
      protected:
        Time *time                                       = nullptr;
        NetworkAccessTechnology *networkAccessTechnology = nullptr;
        SignalStrengthBase *signal                       = nullptr;
        Lock *lock                                       = nullptr;
        SIM *sim                                         = nullptr;
        BatteryBase *battery                             = nullptr;
        HBox *leftBox                                    = nullptr;
        HBox *centralBox                                 = nullptr;
        HBox *rightBox                                   = nullptr;
        Configuration configuration;

        void prepareWidget();

        void showSim(bool enabled);
        void showTime(bool enabled);
        void showLock(bool enabled);
        void showBattery(bool enabled);
        void showSignalStrength(bool enabled);
        void showNetworkAccessTechnology(bool enabled);

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

        bool updateSim();
        bool updateTime();
        bool updateBattery();
        bool updateSignalStrength();
        bool updateNetworkAccessTechnology();

        void accept(GuiVisitor &visitor) override;
    };

} // namespace gui::top_bar
