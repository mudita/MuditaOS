// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ctime>
#include <iomanip>
#include "Label.hpp"
#include "Image.hpp"
#include "TopBar.hpp"
#include <time/time_conversion.hpp>
#include "Style.hpp"
#include "TopBar/BatteryBar.hpp"
#include "TopBar/BatteryText.hpp"
#include "TopBar/SignalStrengthBar.hpp"
#include "TopBar/SignalStrengthText.hpp"
#include "TopBar/NetworkAccessTechnology.hpp"
#include "TopBar/SIM.hpp"
#include "TopBar/Time.hpp"
#include "TopBar/Lock.hpp"
#include "common_data/EventStore.hpp"
#include "time/time_locale.hpp"

namespace gui::top_bar
{
    constexpr auto batteryWidgetAsText = true;
    using BatteryType                  = std::conditional<batteryWidgetAsText, BatteryText, BatteryBar>::type;
    constexpr auto signalWidgetAsText  = true;
    using SignalType = std::conditional<signalWidgetAsText, SignalStrengthText, SignalStrengthBar>::type;

    namespace networkTechnology
    {
        constexpr uint32_t x = 100;
        constexpr uint32_t y = 21;
        constexpr uint32_t w = 130;
        constexpr uint32_t h = 20;
    } // namespace networkTechnology

    static constexpr uint32_t signalOffset  = 20;
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

    auto Configuration::getTimeMode() const noexcept -> TimeMode
    {
        return timeMode;
    }

    void Configuration::set(TimeMode _timeMode)
    {
        timeMode = _timeMode;
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

        preBuildDrawListHook = [this](std::list<Command> &) { updateTime(); };
    }

    void TopBar::prepareWidget()
    {
        battery                      = new BatteryType(this, batteryOffset, 15, 60, 24);
        signal                       = new SignalType(this, signalOffset, 17, 70, 24);
        const auto design_sim_offset = 376; // this offset is not final, but it is pixel Purefect
        sim                          = new SIM(this, design_sim_offset, 12);

        lock = new Lock(this, 240 - 11, 17);

        time = new Time(this, 0, 0, 480, this->drawArea.h);

        networkAccessTechnology = new NetworkAccessTechnology(
            this, networkTechnology::x, networkTechnology::y, networkTechnology::w, networkTechnology::h);

        updateSignalStrength();
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

        using namespace utils::time;
        config.getTimeMode() == TimeMode::Time12h ? time->setFormat(Locale::format(Locale::TimeFormat::FormatTime12H))
                                                  : time->setFormat(Locale::format(Locale::TimeFormat::FormatTime24H));

        for (auto [indicator, enabled] : config.getIndicatorsConfiguration()) {
            setIndicatorStatus(indicator, enabled);
        }
        configuration = std::move(config);
    }

    void TopBar::setIndicatorStatus(Indicator indicator, bool enabled)
    {
        switch (indicator) {
        case Indicator::Signal:
            showSignalStrength(enabled);
            break;
        case Indicator::Time:
            showTime(enabled);
            break;
        case Indicator::Lock:
            showLock(enabled);
            break;
        case Indicator::Battery:
            showBattery(enabled);
            break;
        case Indicator::SimCard:
            showSim(enabled);
            break;
        case Indicator::NetworkAccessTechnology:
            showNetworkAccessTechnology(enabled);
            break;
        }
    }

    bool TopBar::updateBattery()
    {
        if (battery == nullptr) {
            return false;
        }
        showBattery(configuration.isEnabled(Indicator::Battery));
        return true;
    }

    void TopBar::showBattery(bool enabled)
    {
        battery->update(Store::Battery::get());
        enabled ? battery->show() : battery->hide();
    }

    void TopBar::showSim(bool enabled)
    {
        sim->update(Store::GSM::get()->sim);
        enabled ? sim->show() : sim->hide();
    }

    bool TopBar::updateSim()
    {
        if (sim == nullptr) {
            return false;
        }
        showSim(configuration.isEnabled(Indicator::SimCard));
        return true;
    }

    void TopBar::showSignalStrength(bool enabled)
    {
        auto signalStrength = Store::GSM::get()->getSignalStrength();
        signal->update(signalStrength);
        enabled ? signal->show() : signal->hide();
    }

    bool TopBar::updateSignalStrength()
    {
        if (signal == nullptr) {
            return false;
        }
        showSignalStrength(configuration.isEnabled(Indicator::Signal));
        return true;
    }

    bool TopBar::updateNetworkAccessTechnology()
    {
        if (networkAccessTechnology == nullptr) {
            return false;
        }
        showNetworkAccessTechnology(configuration.isEnabled(Indicator::NetworkAccessTechnology));
        return true;
    }

    void TopBar::showNetworkAccessTechnology(bool enabled)
    {
        networkAccessTechnology->update(Store::GSM::get()->getNetwork().accessTechnology);
        enabled ? networkAccessTechnology->show() : networkAccessTechnology->hide();
    }

    void TopBar::showTime(bool enabled)
    {
        time->update();
        if (enabled) {
            time->show();
            lock->hide();
            return;
        }
        time->hide();
    }

    void TopBar::showLock(bool enabled)
    {
        if (enabled) {
            lock->show();
            time->hide();
            return;
        }
        lock->hide();
    }

    bool TopBar::updateTime()
    {
        if (time == nullptr) {
            return false;
        }
        showTime(configuration.isEnabled(Indicator::Time));
        return true;
    }

    void TopBar::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }
} // namespace gui::top_bar
