// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "GuiTimer.hpp"
#include "Item.hpp"                    // for Item
#include "Service/Timer.hpp"           // for Timer, Timer::Type, Timer::Ty...
#include "module-apps/Application.hpp" // for Application
#include <memory>                      // for allocator

namespace app
{
    namespace
    {
        constexpr auto toSysTimerType(gui::Timer::Type type) noexcept -> sys::Timer::Type
        {
            if (type == gui::Timer::Type::Single) {
                return sys::Timer::Type::SingleShot;
            }
            return sys::Timer::Type::Periodic;
        }
    } // namespace

    void GuiTimer::start()
    {
        sys::Timer::start();
    }

    void GuiTimer::stop()
    {
        sys::Timer::stop();
    }

    void GuiTimer::reset()
    {
        sys::Timer::start();
    }

    void GuiTimer::setInterval(gui::ms time)
    {
        sys::Timer::setInterval(time);
    }

    GuiTimer::GuiTimer(Application *parent) : GuiTimer("GUI", parent)
    {}

    GuiTimer::GuiTimer(const std::string &name, Application *parent, gui::ms timeout, gui::Timer::Type type)
        : sys::Timer(name, parent, timeout, toSysTimerType(type)), sysapi{*this}
    {}

    void GuiTimer::Sysapi::connect(gui::Item *item)
    {
        if (item != nullptr) {
            parent.connect([item, this](sys::Timer &timer) { item->onTimer(parent); });
        }
    }
} // namespace app
