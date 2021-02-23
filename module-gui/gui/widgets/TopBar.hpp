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

namespace gui
{
    class SignalStrengthWidgetBase;
    class BatteryWidgetBase;
    class NetworkAccessTechnologyWidget;
} // namespace gui
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
        static uint32_t time;

      protected:
        Label *timeLabel                                             = nullptr;
        NetworkAccessTechnologyWidget *networkAccessTechnologyWidget = nullptr;
        SignalStrengthWidgetBase *signalWidget                       = nullptr;
        Image *lock;
        gui::SIM *sim                    = nullptr;
        BatteryWidgetBase *batteryWidget = nullptr;
        Configuration configuration;

        void prepareWidget();

        void showSim(bool enabled);

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
         * @brief Sets charge level of the battery. This will cause appropriate image to be
         * displayed.
         * @return if display should be refreshed or not
         */
        bool updateBattery();

        /**
         * @brief updates signal strength. This will cause appropriate image to be displayed.
         */
        bool updateSignalStrength();

        bool updateNetworkAccessTechnology();

        void simSet();

        void setTime(const UTF8 &value);

        void accept(GuiVisitor &visitor) override;
    };

} // namespace gui::top_bar
