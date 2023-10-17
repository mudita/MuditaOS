// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

#include <map>
#include <string>
#include <memory>
#include <optional>
#include <purefs/fs/fsnotify.hpp>
#include <purefs/fs/inotify_flags.hpp>

namespace sys
{
    class Service;
}
namespace cpp_freertos
{
    class MutexRecursive;
}
namespace purefs::fs::internal
{
    //! Internal class related to the notify VFS events
    class notifier
    {
        //! Container for path item
        struct path_item
        {
            path_item(std::string_view _path, bool _read_only) : path(_path), read_only(_read_only)
            {}
            const std::string path;
            const bool read_only;
        };
        //! Container for service and subscribed events
        struct service_item
        {
            service_item(std::weak_ptr<sys::Service> _service, inotify_flags _subscribed_events)
                : service(_service), subscribed_events(_subscribed_events)
            {}
            const std::weak_ptr<sys::Service> service;
            const inotify_flags subscribed_events;
        };
        //! Container for the the event
        using container_t = std::multimap<std::string, service_item>;

      public:
        notifier();
        notifier(notifier &) = delete;
        notifier &operator=(notifier &) = delete;
        virtual ~notifier();
        //! Iterator for the registered event
        using item_it = container_t::iterator;
        /**
         * @brief Register selected path for the monitoring
         *
         * @param path Path for monitor
         * @param owner Service which should be notified
         * @param flags Event mask which should be monitored
         * @return std::optional<item_it>  Registered event iterator or nothing if failed
         */
        auto register_path(std::string_view path, std::shared_ptr<sys::Service> owner, inotify_flags flags)
            -> std::optional<item_it>;
        /**
         * @brief Unregister selected path from monitoring
         *
         * @param item Iterator returned by the @see register_path method
         */
        auto unregister_path(item_it item) -> void;
        /**
         * @brief Internal method called on file open
         *
         * @param path Open file path
         * @param fd File descriptor assigned by open method
         * @param ro File is opened in read only mode
         */
        auto notify_open(std::string_view path, int fd, bool ro) const -> void;
        /**
         * @brief Internal method called on closing gile
         *
         * @param fd File descriptor
         */
        auto notify_close(int fd) const -> void;
        /**
         * @brief Notify for event on the file system
         *
         * @param fd File descriptor for file
         * @param mask Filesystem event type
         */
        auto notify(int fd, inotify_flags mask) const -> void;
        /**
         * @brief Notify for event on filesystem on path change
         *
         * @param path Path before event occured
         * @param path_prv Path after event occured
         * @param mask Filesystem event type
         */
        auto notify(std::string_view path, std::string_view path_prv, inotify_flags mask) const -> void;
        /**
         * @brief Notify for event on the filesystem
         *
         * @param path Path related to th event
         * @param mask Filesystem event type
         */
        auto notify(std::string_view path, inotify_flags mask) const -> void
        {
            notify(path, "", mask);
        }

      private:
        /**
         * @brief Private method called for send file monitor event
         *
         * @param svc Target service
         * @param flags Filesystem event type
         * @param name Path related to the event before change
         * @param name_dst Path realated to the event after change
         */
        virtual auto send_notification(std::shared_ptr<sys::Service> svc,
                                       inotify_flags flags,
                                       std::string_view name,
                                       std::string_view name_dst) const -> void;

      private:
        //! Events container
        container_t m_events;
        //! Map file descriptors with path assiociated with it
        mutable std::map<int, path_item> m_fd_map;
        //! Internal mutex for lock the object
        std::unique_ptr<cpp_freertos::MutexRecursive> m_lock;
    };
} // namespace purefs::fs::internal
