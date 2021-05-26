/// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <purefs/fs/notifier.hpp>
#include <purefs/fs/inotify_message.hpp>
#include <functional>
#include <Service/Service.hpp>

namespace purefs::fs::internal
{
    namespace
    {
        void for_path(std::string_view path, std::function<void(std::string_view)> fun)
        {
            constexpr auto sep = '/';
            for (auto it = (path.back() == sep) ? (path.size() - 1) : (path.size()); it; it = path.rfind(sep, it - 1))
                fun(path.substr(0, it));
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
    void notifier::notify(int fd, inotify_flags mask) const
    {
        const auto fname_it = m_fd_map.find(fd);
        if (fname_it != std::end(m_fd_map)) {
            notify(fname_it->second, mask);
        }
    }
    void notifier::notify(std::string_view path, inotify_flags mask) const
    {
        for_path(path, [this, mask](std::string_view path) {
            const auto range = m_events.equal_range(std::string(path));
            for (auto i = range.first; i != range.second; ++i) {
                if (i->second.second && mask) {
                    auto svc = i->second.first.lock();
                    if (svc) {
                        svc->bus.sendUnicast(nullptr, svc->GetName());
                    }
                }
            }
        });
    }
} // namespace purefs::fs::internal