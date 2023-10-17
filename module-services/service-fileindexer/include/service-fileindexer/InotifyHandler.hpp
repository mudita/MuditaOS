// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <purefs/fs/inotify_message.hpp>

#include <memory>
#include <string>
#include <vector>

namespace purefs::fs
{
    class inotify;
}

namespace service::detail
{
    class InotifyHandler
    {
      public:
        InotifyHandler() = default;
        ~InotifyHandler();
        InotifyHandler(const InotifyHandler &)  = delete;
        InotifyHandler(const InotifyHandler &&) = delete;
        InotifyHandler &operator=(const InotifyHandler &) = delete;
        InotifyHandler &operator=(const InotifyHandler &&) = delete;

        bool init(std::shared_ptr<sys::Service> svc);
        void registerMessageHandlers();

        bool addWatch(std::string_view monitoredPath);

      private:
        std::shared_ptr<purefs::fs::inotify> mfsNotifier;
        std::shared_ptr<sys::Service> svc;
        std::vector<std::string_view> monitoredPaths;

        // On update or create content
        void onUpdateOrCreate(std::string_view path);
        // On remove content
        void onRemove(std::string_view path);

        sys::MessagePointer handleInotifyMessage(purefs::fs::message::inotify *inotify);
    };
} // namespace service::detail
