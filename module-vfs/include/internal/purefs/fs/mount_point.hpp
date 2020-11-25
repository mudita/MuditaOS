// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <memory>
#include <string>

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
                    std::shared_ptr<filesystem_operations> fs)
            : m_diskh(diskh), m_path(path), m_fs(fs)
        {}
        auto disk() const noexcept
        {
            return m_diskh;
        }
        auto mount_path() const noexcept
        {
            return m_path;
        }
        auto fs_ops() const noexcept
        {
            return m_fs;
        }

      private:
        const std::weak_ptr<blkdev::internal::disk_handle> m_diskh;
        const std::string_view m_path;                   //! Mounted path
        const std::weak_ptr<filesystem_operations> m_fs; //! Filesystem operation
    };
} // namespace purefs::fs::internal
