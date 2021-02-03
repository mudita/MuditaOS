// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Service/Timer.hpp"             // for Timer
#include <module-gui/gui/core/Timer.hpp> // for ms, Timer
#include <string>                        // for string

namespace app
{
    class Application;
} // namespace app
namespace gui
{
    class Item;
} // namespace gui

namespace app
{

    class Application;

    /// proxies system Timer capabilities to gui::Timer and disconnects dependencies
    /// by default one time run
    class GuiTimer : public gui::Timer, protected sys::Timer
    {
      public:
        /// gui timer default named GUI, infinite timeout on start
        explicit GuiTimer(Application *parent);
        /// gui timer with user name, infinite timeout on start
        GuiTimer(const std::string &name,
                 Application *parent,
                 gui::ms timeout       = sys::Timer::timeout_infinite,
                 gui::Timer::Type type = gui::Timer::Single);
        /// there is no valid reason to create timer without app
        GuiTimer() = delete;

        /// @defgroup interface
        /// @ {
        void start() override;
        void stop() override;
        void reset() override;
        void setInterval(gui::ms time) override;
        /// @ }

        /// interface to trigger timing callback
        class Sysapi
        {
            friend Application;
            GuiTimer &parent;
            Application *app = nullptr;
            void connect(gui::Item *item);

          public:
            Sysapi(GuiTimer &parent, Application *app) : parent(parent), app(app)
            {}
        } sysapi;
    };
}; // namespace app
