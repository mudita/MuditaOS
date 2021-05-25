// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>
#include <memory>

namespace purefs::fs
{

    //! Inotifier object returned by the monitoring API
    class inotify
    {
      public:
        using container_t = std::shared_ptr<inotify>;
        //! Event monitor flag
        enum mask : unsigned
        {
            maccess        = 0x01, //! File is accessed
            mattrib        = 0x02, //! Attribute changed
            mclose_write   = 0x04, //! File closed after write
            mclose_nowrite = 0x08, //! File closed without write
            mdelete        = 0x10, //! File was deleted
            mmodify        = 0x20, //! File modified
            mmove          = 0x40, //! File moved
            mopen          = 0x80  //! File was opended
        };
        /**  Add path for monitoring for monitoring
         * @param[in] monitored_path Path or file which should be monitored
         * @param[in] event_mask Event mask for file monitor
         * @return Error code
         */
        int add_watch(std::string_view monitored_path, mask event_mask);
        /**
         * @param[in] monitored_path Monitored path for removal
         * @return Error code
         */
        int rm_watch(std::string_view monitored_path);
    };
} // namespace purefs::fs
