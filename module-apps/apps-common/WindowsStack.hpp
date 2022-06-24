// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <algorithm>
#include <memory>
#include <map>
#include <optional>
#include <vector>
#include <string>
#include <ostream>
#include <log/log.hpp>
#include "popups/Disposition.hpp"

namespace gui
{
    class AppWindow;
}

namespace app
{

    class WindowsFactory;
    class ApplicationCommon;

    constexpr auto previousWindow = 1;
    constexpr auto topWindow      = 0;

    struct WindowData
    {
      public:
        std::string name;
        gui::popup::Disposition disposition{};

        WindowData(const std::string &name, const gui::popup::Disposition &disposition)
            : name(name), disposition(disposition)
        {}
    };

    class WindowsStack
    {
        std::map<std::string, std::unique_ptr<gui::AppWindow>> windows{};
        std::vector<WindowData> stack;
        decltype(stack)::iterator findInStack(const std::string &);

      public:
        WindowsStack() = default;

        /// iterators, unfortunately used in applications
        std::map<std::string, std::unique_ptr<gui::AppWindow>>::const_iterator begin() const;
        std::map<std::string, std::unique_ptr<gui::AppWindow>>::const_iterator end() const;
        [[nodiscard]] bool isEmpty() const noexcept;

        /// add window on top of stack
        void push(const std::string &name,
                  std::unique_ptr<gui::AppWindow> window,
                  const gui::popup::Disposition &disposition = gui::popup::WindowDisposition);

        /// window getters
        gui::AppWindow *get(const std::string &name) const;
        std::optional<std::string> get(uint32_t depth = 0) const;
        std::optional<WindowData> getWindowData(uint32_t depth = 0) const;

        /// functions removing windows from stack
        /// `pop`  functions - handle last, latest window
        /// `drop` functions - remove any window on stack
        /// return false on pop empty
        bool pop() noexcept;
        bool pop(const std::string &window);
        bool popLastWindow();
        bool drop(const std::string &window);
        void dropPendingPopups();
        void clear();

        bool isWindowOnStack(const std::string &window);

        bool rebuildWindows(app::WindowsFactory &windowsFactory, ApplicationCommon *app);
    };

} // namespace app
