// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ctime>
#include <iomanip>
#include "Label.hpp"
#include "Image.hpp"
#include "TopBar.hpp"
#include <time/time_conversion.hpp>
#include "Style.hpp"

#include "common_data/EventStore.hpp"

namespace gui
{
    namespace networkTechnology
    {
        constexpr uint32_t x = 80;
        constexpr uint32_t y = 21;
        constexpr uint32_t w = 130;
        constexpr uint32_t h = 20;
    } // namespace networkTechnology
    const uint32_t TopBar::signalOffset    = 35;
    const uint32_t TopBar::batteryOffset   = 413;
    gui::TopBar::TimeMode TopBar::timeMode = TimeMode::TIME_24H;
    uint32_t TopBar::time                  = 0;

    TopBar::TopBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Rect{parent, x, y, w, h}
    {

        prepareWidget();

        setFillColor(ColorFullWhite);
        setBorderColor(ColorNoColor);
        setFilled(true);
        setSize(480, 50);
        updateDrawArea();

        preBuildDrawListHook = [this](std::list<Command> &) {
            setTime(time, (timeMode == TimeMode::TIME_24H) ? true : false);
        };
    }

    void TopBar::batteryShowBars(uint32_t val)
    {
        if (val > battery.size()) {
            LOG_ERROR("Trying to set battery level out of scope");
            val = battery.size();
        }
        for (unsigned int i = 0; i < battery.size(); ++i) {
            if (elements.battery) {
                battery[i]->setVisible(val >= i);
            }
            else {
                battery[i]->setVisible(false);
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
        battery = {
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery_low_W_M"),
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery1_W_M"),
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery2_W_M"),
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery3_W_M"),
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery4_W_M"),
            new gui::Image(this, batteryOffset, 15, 0, 0, "battery5_W_M"),
        };
        batteryShowBars(0);

        charging = new Label(this, batteryOffset, 15, 30, this->drawArea.h);
        charging->setFilled(false);
        charging->setBorderColor(gui::ColorNoColor);
        charging->setFont(style::header::font::title);
        charging->setText("Z");
        charging->setVisible(false);

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

        networkAccessTechnologyLabel =
            new Label(this, networkTechnology::x, networkTechnology::y, networkTechnology::w, networkTechnology::h);
        networkAccessTechnologyLabel->setFilled(false);
        networkAccessTechnologyLabel->setBorderColor(gui::ColorNoColor);
        networkAccessTechnologyLabel->setFont(style::header::font::modes);
        networkAccessTechnologyLabel->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        updateNetworkAccessTechnology();
    }

    void TopBar::setActive(std::list<std::pair<TopBar::Elements, bool>> elements)
    {
        for (auto el : elements) {
            setActive(el.first, el.second);
        }
    }

    void TopBar::setActive(TopBar::Elements element, bool active)
    {
        switch (element) {
        case Elements::BATTERY: {
            elements.battery = active;
            if (Store::Battery::get().state == Store::Battery::State::Discharging) {
                setBatteryLevel(active ? Store::Battery::get().level : 0);
            }
            else {
                if (active) {
                    setBatteryCharging(true);
                }
                else {
                    charging->setVisible(false);
                    setBatteryLevel(0);
                }
            }
        } break;
        case Elements::LOCK: {
            elements.lock = active;
            lock->setVisible(active);
            if (active)
                timeLabel->setVisible(false);
        } break;
        case Elements::SIGNAL: {
            elements.signal = active;
            updateSignalStrength();
        } break;
        case Elements::TIME: {
            elements.time = active;
            timeLabel->setVisible(active);
            if (active)
                lock->setVisible(false);
        } break;
        case Elements::SIM:
            elements.sim = active;
            simSet();
            break;
        case Elements::NETWORK_ACCESS_TECHNOLOGY:
            elements.networkAccessTechnology = active;
            updateNetworkAccessTechnology();
            break;
        };
    }

    uint32_t calculateBatteryLavel(uint32_t percentage)
    {
        uint32_t level = 0;
        if (percentage <= 5)
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

        if (level >= batteryLevelCount) {
            LOG_ERROR("Battery level calculations are done wrong!");
            return batteryLevelCount - 1;
        }
        return level;
    }

    bool TopBar::setBatteryLevel(uint32_t percent)
    {
        if (Store::Battery::get().state != Store::Battery::State::Discharging) {
            return false;
        }
        charging->setVisible(false);
        batteryShowBars(calculateBatteryLavel(percent));
        return true;
    }

    void TopBar::setBatteryCharging(bool plugged)
    {
        if (plugged) {
            batteryShowBars(0);
        }
        if (charging == nullptr)
            return;
        if (plugged) {
            charging->setVisible(true);
            batteryShowBars(0);
        }
        else {
            charging->setVisible(false);
            setBatteryLevel(Store::Battery::get().level);
        }
    }

    bool TopBar::updateSignalStrength()
    {
        for (uint32_t i = 0; i < signalImgCount; i++) {
            signal[i]->setVisible(false);
        }
        if (elements.signal) {
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
        if (elements.networkAccessTechnology) {
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

    void TopBar::setTime(const UTF8 &time)
    {
        timeLabel->setText(time);
    }

    void TopBar::setTime(const uint32_t &time, bool mode24H)
    {
        setTime(utils::time::Time());
        timeMode   = (mode24H ? TimeMode::TIME_24H : TimeMode::TIME_12H);
        this->time = time;
    }

    UTF8 TopBar::getTimeString()
    {
        setTime(time, (timeMode == TimeMode::TIME_24H) ? true : false);
        return timeLabel->getText();
    }


    void TopBar::simSet()
    {
        if (!sim) {
            return;
        }
        else if (elements.sim) {
            return sim->show(Store::GSM::get()->sim);
        }
        sim->visible = false;
    }

    void TopBar::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }
} /* namespace gui */
