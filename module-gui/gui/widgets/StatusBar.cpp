// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <iomanip>
#include "Label.hpp"
#include "Image.hpp"
#include "BoxLayout.hpp"
#include "StatusBar.hpp"
#include "status-bar/Style.hpp"
#include "status-bar/BatteryBar.hpp"
#include "status-bar/BatteryText.hpp"
#include "status-bar/SignalStrengthBar.hpp"
#include "status-bar/SignalStrengthText.hpp"
#include "status-bar/NetworkAccessTechnology.hpp"
#include "status-bar/PhoneMode.hpp"
#include "status-bar/AlarmClock.hpp"
#include "status-bar/BT.hpp"
#include "status-bar/SIM.hpp"
#include "status-bar/Time.hpp"
#include "status-bar/Lock.hpp"
#include <EventStore.hpp>

#if DEVELOPER_SETTINGS_OPTIONS == 1
gui::ImageTypeSpecifier style::status_bar::imageTypeSpecifier = gui::ImageTypeSpecifier::W_G;
#endif

namespace gui::status_bar
{
    using namespace style::status_bar;

    constexpr auto batteryWidgetAsText = false;
    using BatteryType                  = std::conditional<batteryWidgetAsText, BatteryText, BatteryBar>::type;
    constexpr auto signalWidgetAsText  = false;
    using SignalType = std::conditional<signalWidgetAsText, SignalStrengthText, SignalStrengthBar>::type;
    constexpr auto simWidgetInDevelopersMode = false;
    using SimType = std::conditional<simWidgetInDevelopersMode, SIMDevelopersMode, SIM>::type;

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

    auto Configuration::getPhoneMode() const noexcept -> sys::phone_modes::PhoneMode
    {
        return mPhoneMode;
    }

    void Configuration::setPhoneMode(sys::phone_modes::PhoneMode phoneMode)
    {
        mPhoneMode = phoneMode;
    }

    auto Configuration::getBluetoothMode() const noexcept -> sys::bluetooth::BluetoothMode
    {
        return mBluetoothMode;
    }

    void Configuration::setBluetoothMode(sys::bluetooth::BluetoothMode bluetoothMode)
    {
        mBluetoothMode = bluetoothMode;
    }

    void Configuration::setAlarmClockStatus(bool alarmClockStatus)
    {
        mAlarmClockStatus = alarmClockStatus;
    }

    auto Configuration::getAlarmClockStatus() const noexcept -> bool
    {
        return mAlarmClockStatus;
    }

    auto Configuration::isEnabled(Indicator indicator) const -> bool
    {
        return indicatorStatuses.at(indicator);
    }

    auto Configuration::getIndicatorsConfiguration() const noexcept -> const IndicatorStatuses &
    {
        return indicatorStatuses;
    }

    auto Configuration::getIndicatorsModifiers() const noexcept -> const IndicatorsModifiers &
    {
        return indicatorsModifiers;
    }

    void Configuration::setIndicatorModifier(Indicator indicator, std::shared_ptr<StatusBarVisitor> config)
    {
        indicatorsModifiers[indicator] = std::move(config);
    }

    StatusBar::StatusBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : HBox{parent, x, y, w, h}
    {
        prepareWidget();

        setEdges(RectangleEdge::None);
        setAlignment(Alignment(Alignment::Horizontal::Center));
        updateDrawArea();

        preBuildDrawListHook = [this](std::list<Command> &) { updateTime(); };
    }

    void StatusBar::prepareWidget()
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
        centralBox->setMinimumWidth(boxes::center::minX);
        lock = new Lock(centralBox, 0, 0);
        lock->setMargins(Margins(0, 0, 0, margins::iconBottom));
        time = new Time(centralBox, 0, 0, 0, 0);
        time->setMargins(gui::Margins(0, 0, 0, time::bottomPadding));
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
        sim = new SimType(rightBox, 0, 0);
        sim->setMargins(gui::Margins(0, 0, margins::between, margins::iconBottom));
        bluetooth = new BT(rightBox, 0, 0);
        bluetooth->setMargins(gui::Margins(0, 0, margins::between, margins::iconBottom));
        alarmClock = new AlarmClock(rightBox, 0, 0);
        alarmClock->setMargins(gui::Margins(0, 0, margins::between, margins::iconBottom));

        updateSignalStrength();
        updateNetworkAccessTechnology();
        updatePhoneMode();
    }

    auto StatusBar::getConfiguration() const noexcept -> const Configuration &
    {
        return configuration;
    }

    void StatusBar::configure(Configuration &&config)
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

        if (config.isEnabled(Indicator::Bluetooth)) {
            bluetooth->setBluetoothMode(config.getBluetoothMode());
        }

        if (config.isEnabled(Indicator::AlarmClock)) {
            alarmClock->setAlarmClockStatus(config.getAlarmClockStatus());
        }

        for (auto &[indicator, modifier] : config.getIndicatorsModifiers()) {
            setIndicatorModifier(indicator, *modifier);
        }

        for (auto [indicator, enabled] : config.getIndicatorsConfiguration()) {
            setIndicatorStatus(indicator, enabled);
        }
        configuration = std::move(config);

        leftBox->resizeItems();
        resizeItems();
    }

    void StatusBar::setIndicatorStatus(Indicator indicator, bool enabled)
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
        case Indicator::Bluetooth:
            showBluetooth(enabled);
            break;
        case Indicator::AlarmClock:
            showAlarmClock(enabled);
            break;
        case Indicator::NetworkAccessTechnology:
            showNetworkAccessTechnology(enabled);
            break;
        case Indicator::PhoneMode:
            showPhoneMode(enabled);
            break;
        }
    }

    void StatusBar::setIndicatorModifier(Indicator indicator, StatusBarVisitor &modifier)
    {
        if (indicator == Indicator::SimCard && sim != nullptr) {
            sim->acceptStatusBarVisitor(modifier);
        }
    }

    bool StatusBar::updateBattery()
    {
        if (battery == nullptr) {
            return false;
        }
        return showBattery(configuration.isEnabled(Indicator::Battery));
    }

    bool StatusBar::showBattery(bool enabled)
    {
        const auto visibilityChanged = battery->isVisible() == enabled ? false : true;
        enabled ? battery->show() : battery->hide();
        const auto stateChanged    = battery->update(Store::Battery::get());
        const auto refreshRequired = stateChanged || visibilityChanged;

        return refreshRequired;
    }

    void StatusBar::showBluetooth(bool enabled)
    {
        if (enabled && configuration.getBluetoothMode() != sys::bluetooth::BluetoothMode::Disabled) {
            bluetooth->show();
        }
        else {
            bluetooth->hide();
        }
        rightBox->resizeItems();
    }

    void StatusBar::showAlarmClock(bool enabled)
    {
        if (enabled && configuration.getAlarmClockStatus()) {
            alarmClock->show();
        }
        else {
            alarmClock->hide();
        }
        rightBox->resizeItems();
    }

    void StatusBar::showSim(bool enabled)
    {
        if (enabled) {
            sim->update();
            sim->show();
        }
        else {
            sim->hide();
        }
        rightBox->resizeItems();
    }

    bool StatusBar::updateBluetooth(sys::bluetooth::BluetoothMode mode)
    {
        if (bluetooth == nullptr || !configuration.isEnabled(Indicator::Bluetooth)) {
            return false;
        }
        configuration.setBluetoothMode(mode);
        bluetooth->setBluetoothMode(mode);
        rightBox->resizeItems();
        return true;
    }

    bool StatusBar::updateAlarmClock(bool status)
    {
        if (alarmClock == nullptr || !configuration.isEnabled(Indicator::AlarmClock)) {
            return false;
        }
        configuration.setAlarmClockStatus(status);
        alarmClock->setAlarmClockStatus(status);
        rightBox->resizeItems();
        return true;
    }

    bool StatusBar::updateSim()
    {
        if (sim == nullptr) {
            return false;
        }
        showSim(configuration.isEnabled(Indicator::SimCard));
        return true;
    }

    void StatusBar::showSignalStrength(bool enabled)
    {
        const auto signalStrength = Store::GSM::get()->getSignalStrength();
        const auto networkStatus  = Store::GSM::get()->getNetwork().status;
        signal->update(signalStrength, networkStatus);
        enabled ? signal->show() : signal->hide();
    }

    bool StatusBar::updateSignalStrength()
    {
        if (signal == nullptr) {
            return false;
        }
        showSignalStrength(configuration.isEnabled(Indicator::Signal));
        return true;
    }

    bool StatusBar::updatePhoneMode()
    {
        if (phoneMode == nullptr) {
            return false;
        }
        showPhoneMode(configuration.isEnabled(Indicator::PhoneMode));
        return true;
    }

    void StatusBar::showPhoneMode(bool enabled)
    {
        enabled ? phoneMode->show() : phoneMode->hide();
    }

    bool StatusBar::updateNetworkAccessTechnology()
    {
        if (networkAccessTechnology == nullptr) {
            return false;
        }
        showNetworkAccessTechnology(configuration.isEnabled(Indicator::NetworkAccessTechnology));
        return true;
    }

    void StatusBar::showNetworkAccessTechnology(bool enabled)
    {
        networkAccessTechnology->update(Store::GSM::get()->getNetwork().accessTechnology);
        enabled ? networkAccessTechnology->show() : networkAccessTechnology->hide();
    }

    bool StatusBar::showTime(bool enabled)
    {
        const auto visibilityChanged = time->isVisible() == enabled ? false : true;
        time->update();
        if (enabled) {
            centralBox->setMinimumSize(boxes::center::maxX, this->drawArea.h);
            time->show();
            lock->hide();
            centralBox->resizeItems();
        }
        else {
            time->hide();
        }
        const auto refreshRequired = visibilityChanged || enabled;
        return refreshRequired;
    }

    void StatusBar::showLock(bool enabled)
    {
        if (enabled) {
            centralBox->setMinimumSize(boxes::center::minX, this->drawArea.h);
            lock->show();
            time->hide();
            return;
        }
        lock->hide();
    }

    bool StatusBar::updateTime()
    {
        if (time == nullptr) {
            return false;
        }
        return showTime(configuration.isEnabled(Indicator::Time));
    }

    void StatusBar::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }
} // namespace gui::status_bar
