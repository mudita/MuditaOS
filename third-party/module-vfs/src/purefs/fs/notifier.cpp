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
            std::string ret;
            if (!path.empty() && path[0] != '/') {
                ret = std::string(internal::get_thread_local_cwd_path()) + "/"s + std::string(path);
            }
            else {
                ret = std::string(path);
            }
            if (!ret.empty() && ret.back() == '/') {
                ret.pop_back();
            }
            return ret;
        }
    } // namespace
    notifier::notifier() : m_lock(std::make_unique<cpp_freertos::MutexRecursive>())
    {}
    notifier::~notifier()
    {}
    auto notifier::register_path(std::string_view path, std::shared_ptr<sys::Service> owner, inotify_flags flags)
        -> std::optional<item_it>
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        const auto abspath = absolute_path(path);
        // # Check if it is already registered for same path
        const auto range = m_events.equal_range(abspath);
        for (auto i = range.first; i != range.second; ++i) {
            if (i->second.service.lock() == owner) {
                return std::nullopt;
            }
        }
        return m_events.emplace(std::make_pair(abspath, service_item(owner, flags)));
    }
    auto notifier::unregister_path(item_it item) -> void
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        m_events.erase(item);
    }
    auto notifier::notify(int fd, inotify_flags mask) const -> void
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        const auto fname_it = m_fd_map.find(fd);
        if (fname_it != std::end(m_fd_map)) {
            notify(fname_it->second.path, mask);
        }
    }
    void notifier::notify(std::string_view path, std::string_view path_prv, inotify_flags mask) const
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        const auto abs_path     = absolute_path(path);
        const auto abs_path_prv = absolute_path(path_prv);
        for_path(abs_path, [this, abs_path, abs_path_prv, mask](std::string_view path) {
            const auto range = m_events.equal_range(std::string(path));
            for (auto i = range.first; i != range.second; ++i) {
                if (i->second.subscribed_events && mask) {
                    auto svc = i->second.service.lock();
                    if (svc) {
                        send_notification(svc, mask, abs_path, abs_path_prv);
                    }
                }
            }
        });
    }
    auto notifier::notify_open(std::string_view path, int fd, bool ro) const -> void
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        m_fd_map.emplace(std::make_pair(fd, path_item(path, ro)));
        notify(path, inotify_flags::open);
    }
    auto notifier::notify_close(int fd) const -> void
    {
        cpp_freertos::LockGuard _lck(*m_lock);
        const auto fname_it = m_fd_map.find(fd);
        if (fname_it != std::end(m_fd_map)) {
            notify(fname_it->first,
                   fname_it->second.read_only ? inotify_flags::close_nowrite : inotify_flags::close_write);
            m_fd_map.erase(fname_it);
        }
    }
    auto notifier::send_notification(std::shared_ptr<sys::Service> svc,
                                     inotify_flags flags,
                                     std::string_view name,
                                     std::string_view name_dst) const -> void
    {
        if (svc->GetHandle() != cpp_freertos::Thread::GetCurrentThreadHandle()) {
            auto msg = std::make_shared<message::inotify>(flags, name, name_dst);
            svc->bus.sendUnicast(std::move(msg), svc->GetName());
        }
        else {
            LOG_WARN("Sent notification to the same thread is forbidded");
        }
    }
} // namespace purefs::fs::internal
