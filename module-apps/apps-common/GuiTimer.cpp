// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "GuiTimer.hpp"
#include "Item.hpp" // for Item

#include <apps-common/ApplicationCommon.hpp> // for Application

#include "Timers/SystemTimer.hpp" // for Timer, Timer::Type, Timer::Ty...
#include "Timers/TimerFactory.hpp"

#include <functional>

namespace app
{
    namespace
    {
        auto guiTypeToSysType(GuiTimer::Type type) noexcept
        {
            if (type == GuiTimer::Type::Periodic) {
                return sys::timer::Type::Periodic;
            }
            return sys::timer::Type::SingleShot;
        }
    } // namespace

    class GuiTimer::Impl
    {
      public:
        Impl(ApplicationCommon *parent,
             gui::Item *item,
             const std::string &name,
             std::chrono::milliseconds timeout,
             Type type);
        std::unique_ptr<sys::timer::SystemTimer> timer;
    };

    GuiTimer::Impl::Impl(ApplicationCommon *parent,
                         gui::Item *item,
                         const std::string &name,
                         std::chrono::milliseconds timeout,
                         Type type)
        : timer{std::make_unique<sys::timer::SystemTimer>(parent, name, timeout, guiTypeToSysType(type))}
    {
        timer->connect([item](sys::Timer &self) { item->onTimer(self); });
    }

    GuiTimer::GuiTimer(ApplicationCommon *parent,
                       gui::Item *item,
                       const std::string &name,
                       std::chrono::milliseconds timeout,
                       Type type)
        : pimpl(std::make_unique<GuiTimer::Impl>(parent, item, name, timeout, type)), item{item}
    {}

    GuiTimer::~GuiTimer() noexcept
    {
        item->detachTimer(*(pimpl->timer));
    }

    void GuiTimer::start()
    {
        pimpl->timer->start();
    }

    void GuiTimer::stop()
    {
        pimpl->timer->stop();
    }

    void GuiTimer::restart(std::chrono::milliseconds newInterval)
    {
        pimpl->timer->restart(newInterval);
    }

    bool GuiTimer::isActive() const noexcept
    {
        return pimpl->timer->isActive();
    }

    sys::TimerHandle GuiTimerFactory::createSingleShotTimer(ApplicationCommon *parent,
                                                            gui::Item *item,
                                                            const std::string &name,
                                                            std::chrono::milliseconds interval)
    {
        return createGuiTimer(parent, item, name, interval, GuiTimer::Type::SingleShot);
    }

    sys::TimerHandle GuiTimerFactory::createPeriodicTimer(ApplicationCommon *parent,
                                                          gui::Item *item,
                                                          const std::string &name,
                                                          std::chrono::milliseconds interval)
    {
        return createGuiTimer(parent, item, name, interval, GuiTimer::Type::Periodic);
    }

    sys::TimerHandle GuiTimerFactory::createGuiTimer(ApplicationCommon *parent,
                                                     gui::Item *item,
                                                     const std::string &name,
                                                     std::chrono::milliseconds interval,
                                                     GuiTimer::Type type)
    {
        auto timer = new GuiTimer{parent, item, name, interval, type};
        parent->connect(timer, item);
        return sys::TimerHandle{timer};
    }
} // namespace app
