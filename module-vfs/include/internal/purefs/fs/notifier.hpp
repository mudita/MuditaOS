// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <map>
#include <string>
#include <memory>
#include <purefs/fs/fsnotify.hpp>
#include <purefs/fs/inotify_flags.hpp>

namespace sys
{
    class Service;
}
namespace purefs::fs::internal
{
    //! Notifier class is internal class related to the notify in the VFS
    class notifier
    {
        using svc_container_t = std::pair<std::weak_ptr<sys::Service>, inotify_flags>;
        using container_t     = std::multimap<std::string, svc_container_t>;

      public:
        notifier(notifier &) = delete;
        using item_it        = container_t::iterator;
        // Register and unregister fir the selected service and path
        auto register_path(std::string_view path, std::shared_ptr<sys::Service> owner, inotify_flags flags) -> item_it;
        auto unregister_path(item_it item) -> void;
        // Handler API-s
        void notify(int fd, inotify_flags mask) const;
        void notify(std::string_view path, inotify_flags mask) const;

      private:
        container_t m_events;
        std::map<int, std::string> m_fd_map;
    };
} // namespace purefs::fs::internal
