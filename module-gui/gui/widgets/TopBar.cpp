// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ctime>
#include <iomanip>
#include "Label.hpp"
#include "Image.hpp"
#include "TopBar.hpp"
#include <time/time_conversion.hpp>
#include "Style.hpp"

#include "common_data/EventStore.hpp"
#include <module-apps/application-calendar/data/dateCommon.hpp>

namespace gui::top_bar
{
    namespace networkTechnology
    {
        constexpr uint32_t x = 80;
        constexpr uint32_t y = 21;
        constexpr uint32_t w = 130;
        constexpr uint32_t h = 20;
    } // namespace networkTechnology

    static constexpr uint32_t signalOffset  = 35;
    static constexpr uint32_t batteryOffset = 413;

    void Configuration::enable(Indicator indicator)
    {
        set(indicator, true);
    }

    void Configuration::enable(const Indicators &indicators)
    {
        for (auto indicator : indicators) {
            enable(indicator);
        }
    }

    void Configuration::disable(Indicator indicator)
    {
        set(indicator, false);
    }

    void Configuration::set(Indicator indicator, bool enabled)
    {
        indicatorStatuses[indicator] = enabled;
    }

    auto Configuration::isEnabled(Indicator indicator) const -> bool
    {
        return indicatorStatuses.at(indicator);
    }

    auto Configuration::getIndicatorsConfiguration() const noexcept -> const IndicatorStatuses &
    {
        return indicatorStatuses;
    }

    TopBar::TopBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Rect{parent, x, y, w, h}
    {
        prepareWidget();

        setFillColor(ColorFullWhite);
        setBorderColor(ColorNoColor);
        setFilled(true);
        setSize(480, 50);
        updateDrawArea();

        preBuildDrawListHook = [this](std::list<Command> &) {
            setTime(TimePointToLocalizedHourMinString(TimePointNow()));
        };
    }

    void TopBar::batteryShowBars(uint32_t val)
    {
        if (val > batteryBars.size()) {
            LOG_ERROR("Trying to set battery level out of scope");
            val = batteryBars.size();
        }
        for (unsigned int i = 0; i < batteryBars.size(); ++i) {
            if (configuration.isEnabled(Indicator::Battery)) {
                batteryBars[i]->setVisible(i == val);
            }
            else {
                batteryBars[i]->setVisible(false);
            }
        }
    }

    void TopBar::prepareWidget()
    {
        signal[0] = new gui::Image(this, signalOffset, 17, 0, 0, "signal0");
        signal[1] = new gui::Image(this, signalOffset, 17, 0, 0, "signal1");
        signal[2] = new gui::Image(this, signalOffset, 17, 0, 0, "signal2");
        signal[3] = new gui::Image(this, signalOffset, 17, 0, 0, "signal3");
        signal[4] = new gui::Image(this, signalOffset, 17, 0, 0, "signal4");
        signal[5] = new gui::Image(this, signalOffset, 17, 0, 0, "signal5");
        updateSignalStrength();

        // icons for battery
        batteryBars = {
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery_low_W_M"),
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery1_W_M"),
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery2_W_M"),
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery3_W_M"),
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery4_W_M"),
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery5_W_M"),
        };
        batteryShowBars(0);

        batteryChargings[Store::Battery::State::Charging] =
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery_charging_W_M");
        batteryChargings[Store::Battery::State::PluggedNotCharging] =
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery_charging_ready_W_M");
        for (auto &el : batteryChargings) {
            el.second->setVisible(false);
        }

        const auto design_sim_offset = 376; // this offset is not final, but it is pixel Purefect
        sim                          = new SIM(this, design_sim_offset, 12);

        // icon of the lock
        lock = new gui::Image(this, 240 - 11, 17, 0, 0, "lock");

        // time label
        timeLabel = new Label(this, 0, 0, 480, this->drawArea.h);
        timeLabel->setFilled(false);
        timeLabel->setBorderColor(gui::ColorNoColor);
        timeLabel->setFont(style::header::font::time);
        timeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        networkAccessTechnologyLabel =
            new Label(this, networkTechnology::x, networkTechnology::y, networkTechnology::w, networkTechnology::h);
        networkAccessTechnologyLabel->setFilled(false);
        networkAccessTechnologyLabel->setBorderColor(gui::ColorNoColor);
        networkAccessTechnologyLabel->setFont(style::header::font::modes);
        networkAccessTechnologyLabel->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        updateNetworkAccessTechnology();
    }

    auto TopBar::getConfiguration() const noexcept -> const Configuration &
    {
        return configuration;
    }

    void TopBar::configure(Configuration &&config)
    {
        if (config.isEnabled(Indicator::Lock)) {
            // In current implementation, lock and time indicators are mutually exclusive.
            // I.e. enabling the lock indicator disables the time indicator.
            config.disable(Indicator::Time);
        }

        for (auto [indicator, enabled] : config.getIndicatorsConfiguration()) {
            setIndicatorStatus(indicator, enabled);
        }
        configuration = std::move(config);
    }

    void TopBar::setIndicatorStatus(Indicator indicator, bool enabled)
    {
        switch (indicator) {
        case Indicator::Signal:
            updateSignalStrength();
            break;
        case Indicator::Time:
            timeLabel->setVisible(enabled);
            if (enabled) {
                lock->setVisible(false);
            }
            break;
        case Indicator::Lock:
            lock->setVisible(enabled);
            if (enabled) {
                timeLabel->setVisible(false);
            }
            break;
        case Indicator::Battery:
            showBattery(enabled);
            break;
        case Indicator::SimCard:
            showSim(enabled);
            break;
        case Indicator::NetworkAccessTechnology:
            updateNetworkAccessTechnology();
            break;
        }
    }

    uint32_t TopBar::calculateBatteryBars(uint32_t percentage)
    {
        uint32_t level = 0;
        if (percentage <= 5) // level critical
            level = 0;
        else if (percentage <= 27)
            level = 1;
        else if (percentage <= 50)
            level = 2;
        else if (percentage <= 73)
            level = 3;
        else if (percentage <= 95)
            level = 4;
        else
            level = 5;

        if (level >= batteryBarsCount) {
            LOG_ERROR("Battery level calculations are done wrong!");
            return batteryBarsCount - 1;
        }
        return level;
    }

    bool TopBar::updateBattery(uint32_t percent)
    {
        showBattery(configuration.isEnabled(Indicator::Battery));
        return true;
    }

    bool TopBar::updateBattery(bool plugged)
    {
        showBattery(configuration.isEnabled(Indicator::Battery));
        return true;
    }

    void TopBar::showBattery(bool shown)
    {
        // hide battery bars icons
        for (const auto &bars : batteryBars) {
            bars->setVisible(false);
        }
        // hide battery charging icons
        for (const auto &charging : batteryChargings) {
            charging.second->setVisible(false);
        }

        if (shown) {
            switch (Store::Battery::get().state) {
            case Store::Battery::State::Discharging:
                batteryShowBars(calculateBatteryBars(Store::Battery::get().level));
                break;
            case Store::Battery::State::Charging:
            case Store::Battery::State::PluggedNotCharging:
                batteryChargings[Store::Battery::get().state]->setVisible(true);
                break;
            }
        }
    }

    void TopBar::showSim(bool enabled)
    {
        if (!enabled) {
            sim->setVisible(false);
            return;
        }
        sim->show(Store::GSM::get()->sim);
    }

    bool TopBar::updateSignalStrength()
    {
        for (uint32_t i = 0; i < signalImgCount; i++) {
            signal[i]->setVisible(false);
        }
        if (configuration.isEnabled(Indicator::Signal)) {
            auto rssiBar = Store::GSM::get()->getSignalStrength().rssiBar;
            if (rssiBar < Store::RssiBar::noOfSupprtedBars) {
                signal[static_cast<size_t>(rssiBar)]->setVisible(true);
                return true;
            }
            return false;
        }
        return true;
    }

    bool TopBar::updateNetworkAccessTechnology()
    {
        if (configuration.isEnabled(Indicator::NetworkAccessTechnology)) {
            auto accessTechnology = Store::GSM::get()->getNetwork().accessTechnology;

            constexpr auto text2g  = "2G";
            constexpr auto text3g  = "3G";
            constexpr auto textLte = "LTE";

            switch (accessTechnology) {
            case Store::Network::AccessTechnology::Gsm:
            case Store::Network::AccessTechnology::GsmWEgprs:
                networkAccessTechnologyLabel->setText(text2g);
                break;
            case Store::Network::AccessTechnology::Utran:
            case Store::Network::AccessTechnology::UtranWHsdpa:
            case Store::Network::AccessTechnology::UtranWHsupa:
            case Store::Network::AccessTechnology::UtranWHsdpaAndWHsupa:
                networkAccessTechnologyLabel->setText(text3g);
                break;
            case Store::Network::AccessTechnology::EUtran:
                networkAccessTechnologyLabel->setText(textLte);
                break;
            case Store::Network::AccessTechnology::Unknown:
                networkAccessTechnologyLabel->setText("");
                break;
            }
        }
        return true;
    }

    void TopBar::setTime(const UTF8 &value)
    {
        timeLabel->setText(value);
    }

    void TopBar::simSet()
    {
        if (sim == nullptr) {
            return;
        }
        showSim(configuration.isEnabled(Indicator::SimCard));
    }

    void TopBar::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }
} /* namespace gui */
