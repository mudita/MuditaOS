// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>
namespace purefs::fs::internal
{

    template <typename T> class handle_mapper
    {
      public:
        T const &operator[](std::size_t index) const
        {
            return data[index];
        }
        T &operator[](std::size_t index)
        {
            return data[index];
        }
        void remove(std::size_t index)
        {
            unused.push_back(index);
        }
        bool exists(std::size_t index) const
        {
            return index < data.size();
        }
        std::size_t insert(T const &value);
        auto begin() const
        {
            return data.begin();
        }
        auto end() const
        {
            return data.end();
        }
        auto size() const
        {
            return data.size();
        }
        auto clear() -> void
        {
            data.clear();
            unused.clear();
        }
        using value_type = T;

      private:
        std::vector<T> data;
        std::vector<std::size_t> unused;
    };

    template <typename T> std::size_t handle_mapper<T>::insert(T const &value)
    {
        if (unused.empty()) {
            data.push_back(value);
            return data.size() - 1;
        }
        const std::size_t result = unused.back();
        unused.pop_back();
        data[result] = value;
        return result;
    }
} // namespace purefs::fs::internal
