// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <deque>
#include <string>
#include <vector>

namespace app::manager
{
    using ApplicationName = std::string;
    class ApplicationStack
    {
      public:
        struct Item
        {
            ApplicationName appName;
            bool isCloseable;
        };
        using Container = std::deque<Item>;

        void push(Item &&item);
        void pop();
        void eraseFirstOf(const ApplicationName &appName) noexcept;
        void clear();

        [[nodiscard]] auto unique() const noexcept -> std::vector<ApplicationName>;
        [[nodiscard]] auto isEmpty() const noexcept -> bool;
        [[nodiscard]] auto size() const noexcept -> Container::size_type;
        [[nodiscard]] auto front() noexcept -> Item &;
        [[nodiscard]] auto front() const noexcept -> const Item &;
        [[nodiscard]] auto find(const ApplicationName &appName) const -> std::vector<Item>;
        [[nodiscard]] auto contains(const ApplicationName &appName) const noexcept -> bool;
        [[nodiscard]] auto getIndexOf(const ApplicationName &appName) const noexcept -> int;
        [[nodiscard]] auto begin() -> Container::iterator;
        [[nodiscard]] auto begin() const -> Container::const_iterator;
        [[nodiscard]] auto end() -> Container::iterator;
        [[nodiscard]] auto end() const -> Container::const_iterator;
        [[nodiscard]] auto operator[](int index) -> Item &;
        [[nodiscard]] auto operator[](int index) const -> const Item &;

      private:
        Container stack;
    };

    bool operator==(const ApplicationStack::Item &lhs, const ApplicationStack::Item &rhs) noexcept;
    bool operator!=(const ApplicationStack::Item &lhs, const ApplicationStack::Item &rhs) noexcept;
    bool operator<(const ApplicationStack::Item &lhs, const ApplicationStack::Item &rhs) noexcept;
    bool operator>(const ApplicationStack::Item &lhs, const ApplicationStack::Item &rhs) noexcept;
} // namespace app::manager
