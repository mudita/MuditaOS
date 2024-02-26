// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "tar/tar.hpp"

#include <vector>
#include <iterator>

namespace
{
    bool isValidDirectoryEntry(const tar::entry &entry)
    {
        return entry.name() != "." and entry.name() != ".." and entry.name() != "...";
    }

    void writeToFile(const tar::entry &entry, const std::filesystem::path &path)
    {
        auto fd = std::fopen(path.c_str(), "wb");
        if (fd == nullptr) {
            throw std::filesystem::filesystem_error("During opening " + path.string(), std::error_code{});
        }

        constexpr std::size_t chunkSize = 256 * 1024;
        auto rawDataBuffer              = std::make_unique<std::uint8_t[]>(chunkSize);

        auto bytesLeft = static_cast<std::size_t>(entry.size());
        while (bytesLeft > 0) {
            const auto bytesToRead = std::min(bytesLeft, chunkSize);
            entry.read(rawDataBuffer.get(), bytesToRead);
            std::fwrite(rawDataBuffer.get(), sizeof(*rawDataBuffer.get()), bytesToRead, fd);
            bytesLeft -= bytesToRead;
        }

        std::fclose(fd);
    }
} // namespace

namespace tar
{
    entry::entry(const std::filesystem::path &path)
    {
        if (mtar_open(&handle, path.c_str(), "rb") != MTAR_ESUCCESS) {
            throw std::filesystem::filesystem_error("During opening tar file " + path.string(), std::error_code{});
        }
        streamBuffer = std::make_unique<char[]>(streamBufferSize);
        setvbuf(handle.stream, streamBuffer.get(), _IOFBF, streamBufferSize);

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
    void entry::read(std::uint8_t *data, const std::size_t size) const
    {
        const auto bytesToRead = std::min(size, static_cast<std::size_t>(tar_header.size));
        if (mtar_read_data(&handle, reinterpret_cast<void *>(data), bytesToRead) != MTAR_ESUCCESS) {
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
                writeToFile(entry, full_path);
            }
        }
    }
} // namespace tar
