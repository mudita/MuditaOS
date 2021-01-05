// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <purefs/blkdev/disk_image.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <sys/stat.h>

namespace purefs::blkdev
{

    disk_image::disk_image(std::string_view image_filename) : m_image_name(image_filename)
    {}

    auto disk_image::probe(unsigned int flags) -> int
    {
        m_filedes = ::open(m_image_name.c_str(), O_RDWR, O_SYNC);
        if (!m_filedes)
            return m_filedes;
        struct stat fst;
        auto ret = ::fstat(m_filedes, &fst);
        if (ret < 0) {
            return -errno;
        }
        m_sectors = fst.st_size / sector_size;
        return 0;
    }
    auto disk_image::cleanup() -> int
    {
        int ret{-EBADFD};
        if (m_filedes) {
            if (m_filedes) {
                ret = ::close(m_filedes);
                if (ret < 0)
                    ret = -errno;
            }
        }
        return ret;
    }
    auto disk_image::write(const void *buf, sector_t lba, std::size_t count) -> int
    {
        auto offs = ::lseek(m_filedes, off_t(lba) * sector_size, SEEK_SET);
        if (offs < 0) {
            return offs;
        }
        auto to_write = count * sector_size;
        auto buf_b    = reinterpret_cast<const uint8_t *>(buf);
        do {
            auto ret = ::write(m_filedes, buf_b, to_write);
            if (ret < 0) {
                return -errno;
            }
            to_write -= ret;
            buf_b += ret;
        } while (to_write > 0);
        return 0;
    }
    auto disk_image::erase(sector_t lba, std::size_t count) -> int
    {
        char buf[count * sector_size];
        std::memset(buf, 0xff, count * sector_size);
        return write(buf, lba, count);
    }
    auto disk_image::read(void *buf, sector_t lba, std::size_t count) -> int
    {
        auto offs = ::lseek(m_filedes, off_t(lba) * sector_size, SEEK_SET);
        if (offs < 0) {
            return offs;
        }
        auto to_read = count * sector_size;
        auto buf_b   = reinterpret_cast<uint8_t *>(buf);
        do {
            auto ret = ::read(m_filedes, buf_b, to_read);
            if (ret < 0) {
                return -errno;
            }
            to_read -= ret;
            buf_b += ret;
        } while (to_read > 0);
        return 0;
    }
    auto disk_image::sync() -> int
    {
        int ret{-EBADFD};
        if (m_filedes) {
            ret = fsync(m_filedes);
            if (ret < 0)
                ret = -errno;
        }
        return ret;
    }
    auto disk_image::status() const -> media_status
    {
        struct stat st;
        auto ret = ::stat(m_image_name.c_str(), &st);
        if (!ret)
            return media_status::nomedia;
        else
            return media_status::healthly;
    }
    auto disk_image::get_info(info_type what) const -> scount_t
    {
        switch (what) {
        case info_type::sector_size:
            return sector_size;
        case info_type::sector_count:
            return m_sectors;
        case info_type::erase_block:
            return 1;
        }
        return -1;
    }
} // namespace purefs::blkdev
