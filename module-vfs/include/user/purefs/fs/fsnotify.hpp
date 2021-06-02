// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <string>
#include <memory>
#include <purefs/fs/inotify_flags.hpp>

namespace sys
{
    class Service;
}
namespace cpp_freertos
{
    class MutexRecursive;
}
namespace purefs::fs
{
    namespace internal
    {
        class notifier;
        struct inotify_container;
    } // namespace internal

    //! Inotifier object returned by the monitoring API
    class inotify
    {
      public:
        ~inotify();
        /** Constructor for the inotify user object
         * @param[in] svc Service owner
         * @param[in] notifier Master notifier object
         */
        inotify(std::shared_ptr<sys::Service> svc, std::shared_ptr<purefs::fs::internal::notifier> notifier);
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
        //! Owning service
        std::weak_ptr<sys::Service> m_svc;
        //! Owner notifier
        std::weak_ptr<internal::notifier> m_notify;
        //! Map for the devices
        std::unique_ptr<internal::inotify_container> m_evlist;
        //! Lock the object
        std::unique_ptr<cpp_freertos::MutexRecursive> m_lock;
    };

} // namespace purefs::fs
