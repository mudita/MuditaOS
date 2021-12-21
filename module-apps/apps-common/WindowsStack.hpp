// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <map>
#include <vector>
#include <string>
#include <AppWindow.hpp>

namespace app
{

    class ApplicationCommon;

    class WindowsStack
    {
        std::function<void(WindowsStack &)> onPopCallback = nullptr;
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

        bool rebuildWindows(app::WindowsFactory &windowsFactory, ApplicationCommon *app);
        void registerOnPopCallback(std::function<void(WindowsStack &)> callback);
    };

} // namespace app
