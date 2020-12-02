// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <list>
#include <string>
#include <type_traits>

namespace utils
{
    template <template <class, class> class Container, class T, class Allocator = std::allocator<T>>
    auto split(T strv, T delims = " ")
    {
        static_assert(std::is_same<T, std::string>::value || std::is_same<T, std::string_view>::value,
                      "std::string or std::string_view expected");
        Container<T, Allocator> output;
        size_t first = 0;

        while (first < strv.size()) {
            const auto second = strv.find_first_of(delims, first);

            if (first != second)
                output.emplace_back(strv.substr(first, second - first));

            if (second == std::string_view::npos)
                break;

            first = second + 1;
        }

        return output;
    }
} // namespace utils
