// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ctime>
#include <iomanip>
#include "Label.hpp"
#include "Image.hpp"
#include "BoxLayout.hpp"
#include "TopBar.hpp"
#include <time/time_conversion.hpp>
#include "TopBar/Style.hpp"
#include "TopBar/BatteryBar.hpp"
#include "TopBar/BatteryText.hpp"
#include "TopBar/SignalStrengthBar.hpp"
#include "TopBar/SignalStrengthText.hpp"
#include "TopBar/NetworkAccessTechnology.hpp"
#include "TopBar/PhoneMode.hpp"
#include "TopBar/SIM.hpp"
#include "TopBar/Time.hpp"
#include "TopBar/Lock.hpp"
#include "common_data/EventStore.hpp"
#include "time/time_locale.hpp"

namespace gui::top_bar
{
    using namespace style::header::status_bar;

    constexpr auto batteryWidgetAsText = true;
    using BatteryType                  = std::conditional<batteryWidgetAsText, BatteryText, BatteryBar>::type;
    constexpr auto signalWidgetAsText  = true;
    using SignalType = std::conditional<signalWidgetAsText, SignalStrengthText, SignalStrengthBar>::type;

    void Configuration::enable(Indicator indicator)
    {
        setIndicator(indicator, true);
    }

    void Configuration::enable(const Indicators &indicators)
    {
        for (auto indicator : indicators) {
            enable(indicator);
        }
    }

    void Configuration::disable(Indicator indicator)
    {
        setIndicator(indicator, false);
    }

    void Configuration::setIndicator(Indicator indicator, bool enabled)
    {
        indicatorStatuses[indicator] = enabled;
    }

    auto Configuration::getTimeMode() const noexcept -> TimeMode
    {
        return mTimeMode;
    }

    auto Configuration::getPhoneMode() const noexcept -> sys::phone_modes::PhoneMode
    {
        return mPhoneMode;
    }

    void Configuration::setTimeMode(TimeMode timeMode)
    {
        mTimeMode = timeMode;
    }

    void Configuration::setPhoneMode(sys::phone_modes::PhoneMode phoneMode)
    {
        mPhoneMode = phoneMode;
    }

    auto Configuration::isEnabled(Indicator indicator) const -> bool
    {
        return indicatorStatuses.at(indicator);
    }

    auto Configuration::getIndicatorsConfiguration() const noexcept -> const IndicatorStatuses &
    {
        return indicatorStatuses;
    }

    TopBar::TopBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : HBox{parent, x, y, w, h}
    {
        prepareWidget();

        setEdges(RectangleEdge::None);
        setAlignment(Alignment(Alignment::Horizontal::Center));
        updateDrawArea();

        preBuildDrawListHook = [this](std::list<Command> &) { updateTime(); };
    }

    void TopBar::prepareWidget()
    {
        // left
        leftBox = new HBox(this, 0, 0, 0, 0);
        leftBox->setMinimumSize(boxes::left::minX, this->drawArea.h);
        leftBox->setMaximumSize(boxes::left::maxX, this->drawArea.h);
        leftBox->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Bottom));
        leftBox->setEdges(RectangleEdge::None);
        signal = new SignalType(leftBox, 0, 0, 0, 0);
        signal->setMargins(gui::Margins(margins::between, 0, 0, margins::iconBottom));
        networkAccessTechnology = new NetworkAccessTechnology(leftBox, 0, 0, 0, 0);
        networkAccessTechnology->setMaximumSize(nat::maxX, this->drawArea.h);
        networkAccessTechnology->setMargins(gui::Margins(margins::between, 0, 0, margins::textBottom));
        phoneMode = new PhoneMode(leftBox, 0, 0, 0, 0);
        phoneMode->setMaximumSize(phonemode::maxX, this->drawArea.h);
        phoneMode->setMargins(gui::Margins(margins::between, 0, 0, margins::textBottom));
        phoneMode->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Bottom));

        // center
        centralBox = new HBox(this, 0, 0, 0, 0);
        centralBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Bottom));
        centralBox->setEdges(RectangleEdge::None);
        lock = new Lock(centralBox, 0, 0);
        lock->setMargins(Margins(0, 0, 0, margins::iconBottom));
        time = new Time(centralBox, 0, 0, 0, 0);
        time->setMaximumSize(time::maxX, this->drawArea.h);

        // right
        rightBox = new HBox(this, 0, 0, 0, 0);
        rightBox->setMinimumSize(boxes::right::minX, this->drawArea.h);
        rightBox->setMaximumSize(boxes::right::maxX, this->drawArea.h);
        rightBox->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Bottom));
        rightBox->setEdges(RectangleEdge::None);
        rightBox->setReverseOrder(true);
        battery = new BatteryType(rightBox, 0, 0, 0, 0);
        battery->setMargins(gui::Margins(0, 0, margins::between, margins::iconBottom));
        sim = new SIM(rightBox, 0, 0);
        sim->setMargins(gui::Margins(0, 0, margins::between, margins::iconBottom));

        updateSignalStrength();
        updateNetworkAccessTechnology();
        updatePhoneMode();
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

        // phone mode and NAT are mutually exclusive.
        if (config.isEnabled(Indicator::NetworkAccessTechnology)) {
            config.disable(Indicator::PhoneMode);
        }

        if (config.isEnabled(Indicator::PhoneMode)) {
            phoneMode->setPhoneMode(config.getPhoneMode());
        }

        using namespace utils::time;
        config.getTimeMode() == TimeMode::Time12h ? time->setFormat(Locale::format(Locale::TimeFormat::FormatTime12H))
                                                  : time->setFormat(Locale::format(Locale::TimeFormat::FormatTime24H));

        for (auto [indicator, enabled] : config.getIndicatorsConfiguration()) {
            setIndicatorStatus(indicator, enabled);
        }
        configuration = std::move(config);

        leftBox->resizeItems();
        resizeItems();
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
        case Indicator::PhoneMode:
            showPhoneMode(enabled);
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

    bool TopBar::updatePhoneMode()
    {
        if (phoneMode == nullptr) {
            return false;
        }
        showPhoneMode(configuration.isEnabled(Indicator::PhoneMode));
        return true;
    }

    void TopBar::showPhoneMode(bool enabled)
    {
        enabled ? phoneMode->show() : phoneMode->hide();
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
            centralBox->setMinimumSize(boxes::center::maxX, this->drawArea.h);
            time->show();
            lock->hide();
            centralBox->resizeItems();
            return;
        }
        time->hide();
    }

    void TopBar::showLock(bool enabled)
    {
        if (enabled) {
            centralBox->setMinimumSize(boxes::center::minX, this->drawArea.h);
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
