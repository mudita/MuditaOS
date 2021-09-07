// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-sys/Timers/Timer.hpp"       // for Timer
#include "module-sys/Timers/TimerHandle.hpp" // for TimerHandle

#include <memory>
#include <string> // for string

namespace gui
{
    class Item;
} // namespace gui

namespace app
{
    class Application;

    /// Proxies system timer capabilities to the gui layer
    class GuiTimer : public sys::Timer
    {
      public:
        enum class Type
        {
            SingleShot,
            Periodic
        };

        GuiTimer(Application *parent,
                 gui::Item *item,
                 const std::string &name,
                 std::chrono::milliseconds timeout,
                 Type type);
        ~GuiTimer() noexcept override;

        void start() override;
        void restart(std::chrono::milliseconds newInterval) override;
        void stop() override;
        bool isActive() const noexcept override;

        class Impl;
        std::unique_ptr<Impl> pimpl;
        gui::Item *item = nullptr;
    };

    class GuiTimerFactory
    {
      public:
        static sys::TimerHandle createSingleShotTimer(Application *parent,
                                                      gui::Item *item,
                                                      const std::string &name,
                                                      std::chrono::milliseconds interval);
        static sys::TimerHandle createPeriodicTimer(Application *parent,
                                                    gui::Item *item,
                                                    const std::string &name,
                                                    std::chrono::milliseconds interval);

      private:
        static sys::TimerHandle createGuiTimer(Application *parent,
                                               gui::Item *item,
                                               const std::string &name,
                                               std::chrono::milliseconds interval,
                                               GuiTimer::Type type);
    };
}; // namespace app
