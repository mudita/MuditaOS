// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <platform/linux/DiskImage.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <sys/stat.h>

namespace purefs::blkdev
{

    disk_image::disk_image(std::string_view image_filename, std::size_t sector_size, hwpart_t num_parts)
        : m_image_name(image_filename), m_sector_size(sector_size), m_syspart_sectors(syspart_size / sector_size),
          m_sysparts(num_parts)
    {
        if (num_parts < 1) {
            throw std::range_error("Number of partitions out of range");
        }
        m_filedes.resize(num_parts, invalid_fd);
        m_sectors.resize(num_parts, m_syspart_sectors);
        m_sectors[0] = 0;
    }

    auto disk_image::probe(unsigned int flags) -> int
    {
        std::lock_guard<std::recursive_mutex> m_lock(m_mtx);
        m_filedes[0] = ::open(m_image_name.c_str(), O_RDWR | O_SYNC);
        if (!m_filedes[0])
            return m_filedes[0];
        struct stat fst;
        auto ret = ::fstat(m_filedes[0], &fst);
        if (ret < 0) {
            return -errno;
        }
        m_sectors[0] = fst.st_size / m_sector_size;
        return fst.st_size % m_sector_size;
    }

    auto disk_image::cleanup() -> int
    {
        std::lock_guard<std::recursive_mutex> m_lock(m_mtx);
        int ret{};
        for (auto &fd : m_filedes)
            if (fd > 0) {
                ret = ::close(fd);
                if (ret < 0)
                    return -errno;
                else
                    fd = invalid_fd;
            }
        return ret;
    }

    auto disk_image::write(const void *buf, sector_t lba, std::size_t count, hwpart_t hwpart) -> int
    {
        std::lock_guard<std::recursive_mutex> m_lock(m_mtx);
        if (!range_valid(lba, count, hwpart)) {
            return -ERANGE;
        }
        const int err = open_and_truncate(hwpart);
        if (err) {
            return err;
        }
        auto offs = ::lseek64(m_filedes[hwpart], off64_t(lba) * off64_t(m_sector_size), SEEK_SET);
        if (offs < 0) {
            return offs;
        }
        auto to_write = count * m_sector_size;
        auto buf_b    = reinterpret_cast<const uint8_t *>(buf);
        do {
            auto ret = ::write(m_filedes[hwpart], buf_b, to_write);
            if (ret < 0) {
                return -errno;
            }
            to_write -= ret;
            buf_b += ret;
        } while (to_write > 0);
        return 0;
    }

    auto disk_image::erase(sector_t lba, std::size_t count, hwpart_t hwpart) -> int
    {
        std::unique_ptr<char[]> buf(new char[count * m_sector_size]);
        std::memset(buf.get(), 0xff, count * m_sector_size);
        return write(buf.get(), lba, count, hwpart);
    }

    auto disk_image::read(void *buf, sector_t lba, std::size_t count, hwpart_t hwpart) -> int
    {
        std::lock_guard<std::recursive_mutex> m_lock(m_mtx);
        if (!range_valid(lba, count, hwpart)) {
            return -ERANGE;
        }
        const int err = open_and_truncate(hwpart);
        if (err) {
            return err;
        }
        auto offs = ::lseek64(m_filedes[hwpart], off64_t(lba) * off64_t(m_sector_size), SEEK_SET);
        if (offs < 0) {
            return offs;
        }
        auto to_read = count * m_sector_size;
        auto buf_b   = reinterpret_cast<uint8_t *>(buf);
        do {
            auto ret = ::read(m_filedes[hwpart], buf_b, to_read);
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
        std::lock_guard<std::recursive_mutex> m_lock(m_mtx);
        int ret{};
        for (auto fd : m_filedes)
            if (fd > 0) {
                ret = fsync(fd);
                if (ret < 0)
                    return -errno;
            }
        return ret;
    }

    auto disk_image::status() const -> media_status
    {
        std::lock_guard<std::recursive_mutex> m_lock(m_mtx);
        struct stat st;
        const auto ret = ::stat(m_image_name.c_str(), &st);
        if (ret < 0)
            return media_status::nomedia;
        else
            return media_status::healthly;
    }

    auto disk_image::get_info(info_type what, hwpart_t hwpart) const -> scount_t
    {
        std::lock_guard<std::recursive_mutex> m_lock(m_mtx);
        if (hwpart >= m_sysparts) {
            return -ERANGE;
        }
        switch (what) {
        case info_type::sector_size:
            return m_sector_size;
        case info_type::sector_count:
            return m_sectors[hwpart];
        case info_type::erase_block:
            return 1;
        }
        return -ENOTSUP;
    }

    auto disk_image::range_valid(sector_t lba, std::size_t count, hwpart_t hwpart) const -> bool
    {
        return (hwpart < m_sysparts) && (lba < m_sectors[hwpart]) && ((lba + count) <= m_sectors[hwpart]);
    }

    auto disk_image::disk_image::open_and_truncate(hwpart_t hwpart) -> int
    {
        if (hwpart >= m_sysparts) {
            return -ERANGE;
        }
        if (hwpart == 0 || m_filedes[hwpart] > 0) {
            return 0;
        }
        using namespace std::string_literals;
        const auto alt_filename = m_image_name + "."s + std::to_string(hwpart);
        m_filedes[hwpart]       = ::open(alt_filename.c_str(), O_RDWR | O_CREAT | O_SYNC, 0644);
        if (m_filedes[hwpart] < 0) {
            return -errno;
        }
        struct stat fst;
        auto ret = ::fstat(m_filedes[hwpart], &fst);
        if (ret < 0) {
            return -errno;
        }
        if (static_cast<unsigned long>(fst.st_size) < m_sectors[hwpart] * m_sector_size) {
            ret = ::ftruncate(m_filedes[hwpart], syspart_size);
            if (ret < 0) {
                return ret;
            }
        }
        return 0;
    }

    auto disk_image::pm_control(pm_state target_state) -> int
    {
        pmState = target_state;
        return 0;
    }
    auto disk_image::pm_read(pm_state &current_state) -> int
    {
        current_state = pmState;
        return 0;
    }

} // namespace purefs::blkdev
