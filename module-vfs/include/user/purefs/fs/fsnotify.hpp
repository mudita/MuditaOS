// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>
#include <memory>
#include <purefs/fs/inotify_flags.hpp>

namespace purefs::fs
{

    //! Inotifier object returned by the monitoring API
    class inotify
    {
      public:
        using container_t = std::shared_ptr<inotify>;

        /**  Add path for monitoring for monitoring
         * @param[in] monitored_path Path or file which should be monitored
         * @param[in] event_mask Event mask for file monitor
         * @return Error code
         */
        int add_watch(std::string_view monitored_path, inotify_flags event_mask);
        /**
         * @param[in] monitored_path Monitored path for removal
         * @return Error code
         */
        int rm_watch(std::string_view monitored_path);

      private:
    };

} // namespace purefs::fs
