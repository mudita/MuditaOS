#pragma once

#include "Service/Timer.hpp" // for Timer
#include "Timer.hpp"         // for ms, Timer
#include <string>            // for string
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
        GuiTimer(Application *parent);
        /// gui timer with user name, infinite timeout on start
        GuiTimer(const std::string &name, Application *parent);
        /// gui timer with user name, variable timeout
        GuiTimer(const std::string &name, Application *parent, gui::ms timeout);
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
            void connect(gui::Item *item);

          public:
            Sysapi(GuiTimer &parent) : parent(parent)
            {}
        } sysapi;
    };
}; // namespace app
