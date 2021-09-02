// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationStack.hpp"

#include <algorithm>

namespace app::manager
{
    bool operator==(const ApplicationStack::Item &lhs, const ApplicationStack::Item &rhs) noexcept
    {
        return lhs.appName == rhs.appName;
    }

    bool operator!=(const ApplicationStack::Item &lhs, const ApplicationStack::Item &rhs) noexcept
    {
        return !operator==(lhs, rhs);
    }

    bool operator<(const ApplicationStack::Item &lhs, const ApplicationStack::Item &rhs) noexcept
    {
        return lhs.appName < rhs.appName;
    }

    bool operator>(const ApplicationStack::Item &lhs, const ApplicationStack::Item &rhs) noexcept
    {
        return lhs.appName > rhs.appName;
    }

    void ApplicationStack::push(Item &&item)
    {
        stack.push_front(std::move(item));
    }

    void ApplicationStack::pop()
    {
        if (!isEmpty()) {
            stack.pop_front();
        }
    }

    void ApplicationStack::eraseFirstOf(const ApplicationName &appName) noexcept
    {
        const auto targetApp =
            std::find_if(stack.begin(), stack.end(), [&appName](const auto &item) { return item.appName == appName; });
        if (targetApp != stack.end()) {
            stack.erase(targetApp);
        }
    }

    void ApplicationStack::clear()
    {
        stack.clear();
    }

    auto ApplicationStack::unique() const noexcept -> std::vector<ApplicationName>
    {
        auto stackCopy = stack;
        std::sort(stackCopy.begin(), stackCopy.end());
        const auto last = std::unique(stackCopy.begin(), stackCopy.end());
        stackCopy.erase(last, stackCopy.end());

        std::vector<ApplicationName> uniqueApps{};
        uniqueApps.reserve(stackCopy.size());
        std::transform(stackCopy.begin(), stackCopy.end(), std::back_inserter(uniqueApps), [](const auto &item) {
            return item.appName;
        });
        return uniqueApps;
    }

    bool ApplicationStack::isEmpty() const noexcept
    {
        return stack.empty();
    }

    auto ApplicationStack::size() const noexcept -> Container::size_type
    {
        return stack.size();
    }

    auto ApplicationStack::front() noexcept -> Item &
    {
        return stack.front();
    }

    auto ApplicationStack::front() const noexcept -> const Item &
    {
        return stack.front();
    }

    auto ApplicationStack::find(const ApplicationName &appName) const -> std::vector<Item>
    {
        std::vector<Item> found;
        std::copy_if(stack.begin(), stack.end(), std::back_inserter(found), [&appName](const auto &item) {
            return item.appName == appName;
        });
        return found;
    }

    auto ApplicationStack::contains(const std::string &appName) const noexcept -> bool
    {
        const auto it =
            std::find_if(stack.begin(), stack.end(), [&appName](const auto &item) { return item.appName == appName; });
        return it != stack.end();
    }

    auto ApplicationStack::getIndexOf(const std::string &appName) const noexcept -> int
    {
        const auto targetApp =
            std::find_if(stack.begin(), stack.end(), [&appName](const auto &item) { return item.appName == appName; });
        if (targetApp == stack.end()) {
            return -1;
        }
        return std::distance(stack.begin(), targetApp);
    }

    auto ApplicationStack::begin() -> Container::iterator
    {
        return stack.begin();
    }

    auto ApplicationStack::begin() const -> Container::const_iterator
    {
        return stack.begin();
    }

    auto ApplicationStack::end() -> Container::iterator
    {
        return stack.end();
    }

    auto ApplicationStack::end() const -> Container::const_iterator
    {
        return stack.end();
    }

    auto ApplicationStack::operator[](int index) -> Item &
    {
        return stack[index];
    }

    auto ApplicationStack::operator[](int index) const -> const Item &
    {
        return stack[index];
    }
} // namespace app::manager
