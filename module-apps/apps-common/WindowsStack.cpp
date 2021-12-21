// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WindowsStack.hpp"

#include <utility>
#include "WindowsFactory.hpp"
#include "windows/AppWindow.hpp"

namespace app
{
    std::map<std::string, std::unique_ptr<gui::AppWindow>>::const_iterator WindowsStack::begin() const
    {
        return std::begin(windows);
    }

    std::map<std::string, std::unique_ptr<gui::AppWindow>>::const_iterator WindowsStack::end() const
    {
        return std::end(windows);
    }

    void WindowsStack::push(const std::string &name,
                            std::unique_ptr<gui::AppWindow> window,
                            const gui::popup::Disposition &disposition)
    {
        windows[name] = std::move(window);
        stack.push_back(WindowData(name, disposition));
    }

    gui::AppWindow *WindowsStack::get(const std::string &name) const
    {
        auto ret = windows.find(name);
        return ret == std::end(windows) ? nullptr : ret->second.get();
    }

    std::optional<WindowData> WindowsStack::getWindowData(uint32_t depth) const
    {
        if (depth >= stack.size()) {
            return std::nullopt;
        }
        return {*std::prev(stack.end(), depth + 1)};
    }

    std::optional<std::string> WindowsStack::get(uint32_t depth) const
    {
        if (auto windowData = getWindowData(depth)) {
            return {windowData->name};
        }
        return std::nullopt;
    }

    [[nodiscard]] bool WindowsStack::isEmpty() const noexcept
    {
        return stack.empty();
    }

    /// return false on pop empty
    bool WindowsStack::pop() noexcept
    {
        if (!stack.empty()) {
            stack.pop_back();
            return true;
        }
        return false;
    }

    bool WindowsStack::pop(const std::string &window)
    {
        auto ret = findInStack(window);
        if (ret != stack.end()) {
            stack.erase(std::next(ret), stack.end());
            if (onPopCallback) {
                onPopCallback(*this);
            }
            return true;
        }
        return false;
    }

    bool WindowsStack::popLastWindow()
    {
        if (stack.size() == 1) {
            stack.clear();
            return true;
        }
        return false;
    }

    bool WindowsStack::drop(const std::string &window)
    {
        auto popWindow = findInStack(window);
        if (popWindow != stack.end()) {
            stack.erase(popWindow);
            return true;
        }
        return false;
    }

    bool WindowsStack::rebuildWindows(app::WindowsFactory &windowsFactory, ApplicationCommon *app)
    {
        if (windows.empty()) {
            return false;
        }
        for (auto &[name, window] : windows) {
            windows[name] = windowsFactory.build(app, name);
        }
        return true;
    }

    void WindowsStack::clear()
    {
        stack.clear();
        windows.clear();
    }

    decltype(WindowsStack::stack)::iterator WindowsStack::findInStack(const std::string &window)
    {
        return std::find_if(stack.begin(), stack.end(), [&](auto &el) { return el.name == window; });
    }
    void WindowsStack::registerOnPopCallback(std::function<void(WindowsStack &)> callback)
    {
        onPopCallback = std::move(callback);
    }
    void WindowsStack::dropPendingPopups()
    {
        auto it = stack.rbegin();
        while (it != stack.rend()) {
            LOG_DEBUG("Current window on stack: %s, type: %s",
                      it->name.c_str(),
                      magic_enum::enum_name(it->disposition.windowtype).data());
            if (it->disposition.windowtype == gui::popup::Disposition::WindowType::Popup) {
                LOG_DEBUG("Erasing: %s", it->name.c_str());
                stack.erase(std::next(it).base());
            }
            std::advance(it, 1);
        }
    }

} // namespace app
