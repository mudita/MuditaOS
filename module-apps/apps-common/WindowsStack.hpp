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
        ApplicationCommon *parent;

      public:
        WindowsStack(ApplicationCommon *parent) : parent(parent)
        {}

        std::vector<std::string> stack;
        std::map<std::string, std::unique_ptr<gui::AppWindow>> windows;

        std::map<std::string, std::unique_ptr<gui::AppWindow>>::const_iterator begin() const
        {
            return std::begin(windows);
        }

        std::map<std::string, std::unique_ptr<gui::AppWindow>>::const_iterator end() const
        {
            return std::end(windows);
        }

        [[nodiscard]] auto getParent() const
        {
            return parent;
        }

        auto push(const std::string &name, std::unique_ptr<gui::AppWindow> window)
        {
            windows[name] = std::move(window);
            stack.push_back(name);
        }

        gui::AppWindow *get(const std::string &name) const
        {
            auto ret = windows.find(name);
            return ret == std::end(windows) ? nullptr : ret->second.get();
        }

        [[nodiscard]] auto isEmpty() const noexcept
        {
            return stack.size() == 0;
        }
    };
} // namespace app
