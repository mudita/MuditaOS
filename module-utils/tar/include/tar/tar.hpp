// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include <microtar.hpp>

namespace tar
{
    class entry
    {
      public:
        ~entry();

        std::size_t size() const;
        std::filesystem::path name() const;
        bool is_file() const;
        bool is_directory() const;

        void read(const std::byte *data, std::size_t size) const;

      private:
        explicit entry(const std::filesystem::path &path);
        friend class iterator;
        mutable mtar_t handle{};
        mtar_header_t tar_header;
    };

    class iterator
    {
      public:
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = entry;
        using pointer           = entry *;
        using reference         = entry &;

        iterator() = default;

        explicit iterator(const std::filesystem::path &path);

        reference operator*() const;
        pointer operator->() const;

        iterator &operator++();
        iterator operator++(int);

        friend bool operator==(const iterator &a, const iterator &b);
        friend bool operator!=(const iterator &a, const iterator &b);

      private:
        std::shared_ptr<entry> entry_;
    };

    /** @brief Enable range-based `for` using iterator.
     *
     *  e.g. `for (const auto& entry : tar::iterator("<tar_name>")) ...`
     */
    inline iterator begin(const iterator &it) noexcept
    {
        return it;
    }

    inline iterator end(iterator) noexcept
    {
        return iterator{};
    }

    /**
     * Unpack contents of tar file
     * @param path path to a tar file
     * @param where where to store contents of a tar file
     */
    void unpack(const std::filesystem::path &path, const std::filesystem::path &where = {});
} // namespace tar
