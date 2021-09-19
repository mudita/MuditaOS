// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/fsnotify.hpp>
#include <purefs/fs/notifier.hpp>
#include <log.hpp>
#include <mutex.hpp>
#include <cerrno>

namespace purefs::fs
{
    namespace internal
    {
        struct inotify_container
        {
            std::map<std::string, internal::notifier::item_it> items;
        };
    } // namespace internal
    inotify::inotify(std::shared_ptr<sys::Service> svc, std::shared_ptr<internal::notifier> notifier)
        : m_svc(svc), m_notify(notifier), m_evlist(std::make_unique<internal::inotify_container>()),
          m_lock(std::make_unique<cpp_freertos::MutexRecursive>())
    {}

    inotify::~inotify()
    {
        const auto notifier = m_notify.lock();
        if (!notifier) {
            LOG_ERROR("Unable lock notifier");
        }
        const auto svc = m_svc.lock();
        if (!svc) {
            LOG_ERROR("Unable lock service");
        }
        for (const auto &[_, val] : m_evlist->items) {
            notifier->unregister_path(val);
        }
    }

    int inotify::add_watch(std::string_view monitored_path, inotify_flags event_mask)
    {
        const auto notifier = m_notify.lock();
        if (!notifier) {
            LOG_ERROR("Unable lock notifier");
            return -ENXIO;
        }
        const auto svc = m_svc.lock();
        if (!svc) {
            LOG_ERROR("Unable lock service");
            return -ENXIO;
        }
        auto it = notifier->register_path(monitored_path, svc, event_mask);
        if (!it) {
            LOG_ERROR("Unable to register path");
            return -EIO;
        }
        cpp_freertos::LockGuard _lck(*m_lock);
        m_evlist->items.emplace(std::make_pair(std::string(monitored_path), *it));
        return {};
    }

    int inotify::rm_watch(std::string_view monitored_path)
    {
        const auto notifier = m_notify.lock();
        if (!notifier) {
            LOG_ERROR("Unable lock notifier");
            return -ENXIO;
        }
        const auto svc = m_svc.lock();
        if (!svc) {
            LOG_ERROR("Unable lock service");
            return -ENXIO;
        }
        cpp_freertos::LockGuard _lck(*m_lock);
        auto it = m_evlist->items.find(std::string(monitored_path));
        if (it == std::end(m_evlist->items)) {
            LOG_ERROR("Unable to find registered path %s", std::string(monitored_path).c_str());
            return -ENOENT;
        }
        notifier->unregister_path(it->second);
        m_evlist->items.erase(it);
        return {};
    }
} // namespace purefs::fs
