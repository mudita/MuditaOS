// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <memory>
#include <string>
#include <iostream>
#include <purefs/fs/mount_flags.hpp>

namespace purefs::blkdev::internal
{
    class disk_handle;
}

namespace purefs::fs
{
    class filesystem_operations;
}

namespace purefs::fs::internal
{
    //! Mount point disk private structure
    class mount_point
    {
      public:
        mount_point(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                    std::string_view path,
                    unsigned flags,
                    std::shared_ptr<filesystem_operations> fs)
            : m_diskh(diskh), m_path(path), m_fs(fs), m_flags(flags)
        {}
        mount_point(const mount_point &) = delete;
        auto operator=(const mount_point &) = delete;
        virtual ~mount_point()              = default;
        auto disk() const noexcept
        {
            return m_diskh.lock();
        }
        auto mount_path() const noexcept
        {
            return m_path;
        }
        auto fs_ops() const noexcept
        {
            return m_fs.lock();
        }
        auto flags() const noexcept -> unsigned
        {
            return m_flags;
        }
        auto is_ro() const noexcept -> bool
        {
            return (m_flags & mount_flags::read_only) == mount_flags::read_only;
        }
        void modify_flags(unsigned flags) noexcept
        {
            m_flags = flags;
        }
        auto native_path(std::string_view full_path) const noexcept -> std::string
        {
            const auto n1 = full_path.find(m_path);
            if (n1 == 0) {
                auto ret = std::string(native_root()).append(full_path.substr(m_path.size()));
                if (ret.empty())
                    ret.append("/");
                return ret;
            }
            else {
                return {};
            }
        }

      private:
        virtual auto native_root() const noexcept -> std::string_view = 0;

      private:
        const std::weak_ptr<blkdev::internal::disk_handle> m_diskh;
        const std::string m_path;                        //! Mounted path
        const std::weak_ptr<filesystem_operations> m_fs; //! Filesystem operation
        volatile unsigned m_flags;
    };
} // namespace purefs::fs::internal
