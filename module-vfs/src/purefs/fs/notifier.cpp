// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <purefs/fs/notifier.hpp>
#include <purefs/fs/inotify_message.hpp>
#include <functional>
#include <Service/Service.hpp>
#include <purefs/fs/inotify_message.hpp>
#include <purefs/fs/thread_local_cwd.hpp>
#include <log/log.hpp>

namespace purefs::fs::internal
{
    namespace
    {
        void for_path(std::string_view path, std::function<void(std::string_view)> fun)
        {
            constexpr auto sep = '/';
            for (auto it = (path.back() == sep) ? (path.size() - 1) : (path.size()); (it && it != std::string::npos);
                 it      = path.rfind(sep, it - 1))
                fun(path.substr(0, it));
        }
        std::string absolute_path(std::string_view path)
        {
            using namespace std::string_literals;
            if (!path.empty() && path[0] != '/') {
                return std::string(internal::get_thread_local_cwd_path()) + "/"s + std::string(path);
            }
            else {
                return std::string(path);
            }
        }
    } // namespace
    auto notifier::register_path(std::string_view path, std::shared_ptr<sys::Service> owner, inotify_flags flags)
        -> item_it
    {
        // # Check if it is already registered for same path
        const auto range = m_events.equal_range(std::string(path));
        for (auto i = range.first; i != range.second; ++i) {
            if (i->second.first.lock() == owner) {
                return std::end(m_events);
            }
        }
        return m_events.emplace(std::make_pair(path, std::make_pair(owner, flags)));
    }
    auto notifier::unregister_path(item_it item) -> void
    {
        m_events.erase(item);
    }
    auto notifier::notify(int fd, inotify_flags mask) const -> void
    {
        const auto fname_it = m_fd_map.find(fd);
        if (fname_it != std::end(m_fd_map)) {
            notify(fname_it->second.first, mask);
        }
    }
    void notifier::notify(std::string_view path, std::string_view path_prv, inotify_flags mask) const
    {
        const auto abs_path     = absolute_path(path);
        const auto abs_path_prv = absolute_path(path_prv);
        for_path(abs_path, [this, path_prv, mask](std::string_view path) {
            const auto range = m_events.equal_range(std::string(path));
            for (auto i = range.first; i != range.second; ++i) {
                if (i->second.second && mask) {
                    auto svc = i->second.first.lock();
                    if (svc) {
                        send_notification(svc, mask, path, path_prv);
                    }
                }
            }
        });
    }
    auto notifier::notify_open(std::string_view path, int fd, bool ro) const -> void
    {
        m_fd_map.emplace(std::make_pair(fd, std::make_pair(path, ro)));
        notify(path, inotify_flags::open);
    }
    auto notifier::notify_close(int fd) const -> void
    {
        const auto fname_it = m_fd_map.find(fd);
        if (fname_it != std::end(m_fd_map)) {
            notify(fname_it->first,
                   fname_it->second.second ? inotify_flags::close_nowrite : inotify_flags::close_write);
        }
    }
    auto notifier::send_notification(std::shared_ptr<sys::Service> svc,
                                     inotify_flags flags,
                                     std::string_view name,
                                     std::string_view name_dst) const -> void
    {
        auto msg = std::make_shared<message::inotify>(flags, name, name_dst);
        svc->bus.sendUnicast(std::move(msg), svc->GetName());
    }
} // namespace purefs::fs::internal