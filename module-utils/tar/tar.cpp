// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "tar/tar.hpp"

#include <fstream>
#include <vector>
#include <iterator>

namespace
{
    bool isValidDirectoryEntry(const tar::entry &entry)
    {
        return entry.name() != "." and entry.name() != ".." and entry.name() != "...";
    }

    void write_to_file(const tar::entry &entry, const std::filesystem::path &path)
    {
        constexpr std::size_t chunk_size = 1024 * 128;
        std::ofstream out_file{path, std::ios::binary};
        if (not out_file.is_open()) {
            throw std::filesystem::filesystem_error("During opening " + path.string(), std::error_code{});
        }

        std::vector<std::byte> raw_data(chunk_size);
        auto bytes_left = entry.size();
        while (bytes_left > 0) {
            const std::size_t to_read = bytes_left > chunk_size ? chunk_size : bytes_left;
            entry.read(raw_data.data(), to_read);

            out_file.write(reinterpret_cast<const char *>(raw_data.data()), to_read);
            bytes_left -= to_read;
        }
    }
} // namespace

namespace tar
{
    entry::entry(const std::filesystem::path &path)
    {
        if (mtar_open(&handle, path.c_str(), "rb") != MTAR_ESUCCESS) {
            throw std::filesystem::filesystem_error("During opening tar file " + path.string(), std::error_code{});
        }
        if (mtar_read_header(&handle, &tar_header) != MTAR_ESUCCESS) {
            throw std::filesystem::filesystem_error("During reading from tar file " + path.string(), std::error_code{});
        }
    }
    entry::~entry()
    {
        mtar_close(&handle);
    }
    std::size_t entry::size() const
    {
        return tar_header.size;
    }
    bool entry::is_file() const
    {
        return tar_header.type == MTAR_TREG;
    }
    bool entry::is_directory() const
    {
        return tar_header.type == MTAR_TDIR;
    }
    std::filesystem::path entry::name() const
    {
        return tar_header.name;
    }
    void entry::read(const std::byte *data, const std::size_t size) const
    {
        const std::size_t to_read = size > tar_header.size ? tar_header.size : size;
        if (mtar_read_data(&handle, (void *)(data), to_read) != MTAR_ESUCCESS) {
            throw std::filesystem::filesystem_error("During reading from tar file", std::error_code{});
        }
    }

    iterator::iterator(const std::filesystem::path &path) : entry_{new entry(path)}
    {}
    entry &iterator::operator*() const
    {
        return *entry_;
    }
    iterator::pointer iterator::operator->() const
    {
        return &**this;
    }
    iterator &iterator::operator++()
    {
        if (mtar_next(&entry_->handle) != MTAR_ESUCCESS or
            mtar_read_header(&entry_->handle, &entry_->tar_header) != MTAR_ESUCCESS) {
            entry_ = {};
        }

        return *this;
    }
    iterator iterator::operator++(int)
    {
        iterator tmp = *this;
        ++(*this);
        return tmp;
    }
    bool operator==(const iterator &a, const iterator &b)
    {
        return a.entry_ == b.entry_;
    }
    bool operator!=(const iterator &a, const iterator &b)
    {
        return a.entry_ != b.entry_;
    }

    void unpack(const std::filesystem::path &path, const std::filesystem::path &where)
    {
        for (const auto &entry : iterator(path)) {
            const auto full_path = where / entry.name();
            if (entry.is_directory() and isValidDirectoryEntry(entry)) {
                std::filesystem::create_directories(full_path);
            }
            else if (entry.is_file()) {
                write_to_file(entry, full_path);
            }
        }
    }
} // namespace tar
