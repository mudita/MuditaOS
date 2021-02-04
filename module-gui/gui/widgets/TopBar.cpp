// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ctime>
#include <iomanip>
#include "Label.hpp"
#include "Image.hpp"
#include "TopBar.hpp"
#include <time/time_conversion.hpp>
#include "Style.hpp"
#include "TopBar/BatteryWidgetBar.hpp"
#include "TopBar/BatteryWidgetText.hpp"
#include "TopBar/SignalStrengthWidgetBar.hpp"
#include "TopBar/SignalStrengthWidgetText.hpp"
#include "TopBar/NetworkAccessTechnologyWidget.hpp"
#include "common_data/EventStore.hpp"

namespace gui::top_bar
{
    constexpr auto batteryWidgetAsText = true;
    using BatteryWidgetType = std::conditional<batteryWidgetAsText, BatteryWidgetText, BatteryWidgetBar>::type;
    constexpr auto signalWidgetAsText  = true;
    using SignalWidgetType =
        std::conditional<signalWidgetAsText, SignalStrengthWidgetText, SignalStrengthWidgetBar>::type;

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

        preBuildDrawListHook = [this](std::list<Command> &) { setTime(utils::time::getHoursMinInCurrentTimeFormat()); };
    }

    void TopBar::prepareWidget()
    {
        batteryWidget = new BatteryWidgetType(this, batteryOffset, 15, 60, 24);
        signalWidget  = new SignalWidgetType(this, signalOffset, 17, 70, 24);

        updateSignalStrength();

        const auto design_sim_offset = 376; // this offset is not final, but it is pixel Purefect
        sim                          = new SIM(this, design_sim_offset, 12);

        // icon of the lock
        lock = new gui::Image(this, 240 - 11, 17, 0, 0, "lock");

        // time label
        timeLabel = new Label(this, 0, 0, 480, this->drawArea.h);
        timeLabel->setFilled(false);
        timeLabel->setBorderColor(gui::ColorNoColor);
        timeLabel->setFont(style::header::font::time);
        timeLabel->setText("00:00");
        timeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        networkAccessTechnologyWidget = new NetworkAccessTechnologyWidget(
            this, networkTechnology::x, networkTechnology::y, networkTechnology::w, networkTechnology::h);
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
            if (enabled) {
                signalWidget->show();
                updateSignalStrength();
                break;
            }
            signalWidget->hide();
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
            if (enabled) {
                batteryWidget->show();
                updateBattery();
                break;
            }
            batteryWidget->hide();
            break;
        case Indicator::SimCard:
            showSim(enabled);
            break;
        case Indicator::NetworkAccessTechnology:
            if (enabled) {
                networkAccessTechnologyWidget->show();
                updateNetworkAccessTechnology();
                break;
            }
            networkAccessTechnologyWidget->hide();
            break;
        }
    }

    bool TopBar::updateBattery()
    {
        batteryWidget->update(Store::Battery::get());
        return true;
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
        auto signalStrength = Store::GSM::get()->getSignalStrength();
        signalWidget->update(signalStrength);

        return true;
    }

    bool TopBar::updateNetworkAccessTechnology()
    {
        auto accessTechnology = Store::GSM::get()->getNetwork().accessTechnology;
        networkAccessTechnologyWidget->update(accessTechnology);
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
} // namespace gui::top_bar
