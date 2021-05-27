// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <map>
#include <string>
#include <memory>
#include <purefs/fs/fsnotify.hpp>
#include <purefs/fs/inotify_flags.hpp>
#include <thread.hpp>

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
        notifier()           = default;
        notifier(notifier &) = delete;
        virtual ~notifier()  = default;
        using item_it        = container_t::iterator;
        // Register and unregister fir the selected service and path
        auto register_path(std::string_view path, std::shared_ptr<sys::Service> owner, inotify_flags flags) -> item_it;
        auto unregister_path(item_it item) -> void;
        // Handler API-s
        auto notify_open(std::string_view path, int fd, bool ro) const -> void;
        auto notify_close(int fd) const -> void;
        auto notify(int fd, inotify_flags mask) const -> void;
        auto notify(std::string_view path, std::string_view path_prv, inotify_flags mask) const -> void;
        auto notify(std::string_view path, inotify_flags mask) const -> void
        {
            notify(path, "", mask);
        }

      private:
        virtual auto send_notification(std::shared_ptr<sys::Service> svc,
                                       inotify_flags flags,
                                       std::string_view name,
                                       std::string_view name_dst) const -> void;

      private:
        container_t m_events;
        mutable std::map<int, std::pair<std::string, bool>> m_fd_map;
        TaskHandle_t threadHandle;
    };
} // namespace purefs::fs::internal
