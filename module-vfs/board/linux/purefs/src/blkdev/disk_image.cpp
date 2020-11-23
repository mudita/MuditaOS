// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <purefs/blkdev/disk_image.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

namespace purefs::blkdev
{

    auto disk_image::probe(unsigned int flags) -> int
    {
        m_filedes = ::open(file_name, O_RDWR, O_SYNC);
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
        int ret = ::lseek(m_filedes, lba * sector_size, SEEK_SET);
        if (ret < 0) {
            return ret;
        }
        auto to_write = lba * sector_size;
        auto buf_b    = reinterpret_cast<const uint8_t *>(buf);
        do {
            ret = ::write(m_filedes, buf_b, to_write);
            if (ret < 0) {
                return ret;
            }
            to_write -= ret;
            buf_b += ret;
        } while (to_write > 0);
        return 0;
    }
    auto disk_image::read(void *buf, sector_t lba, std::size_t count) -> int
    {
        int ret = ::lseek(m_filedes, lba * sector_size, SEEK_SET);
        if (ret < 0) {
            return ret;
        }
        auto to_read = lba * sector_size;
        auto buf_b   = reinterpret_cast<uint8_t *>(buf);
        do {
            ret = ::read(m_filedes, buf_b, to_read);
            if (ret < 0) {
                return ret;
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
        auto ret = ::stat(file_name, &st);
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
            return 0;
        }
        return -1;
    }
} // namespace purefs::blkdev
