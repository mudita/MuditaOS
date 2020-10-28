// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <Service/Timer.hpp>

#include <list>

namespace service::msg
{
    class FileChangeMessage;
}
namespace service::detail
{
    // RFC6838 IANA MIME types
    enum class mimeType
    {
        _none_,
        application,
        audio,
        example,
        font,
        image,
        text,
        video
    };
    class startupIndexer
    {
        static constexpr auto timer_indexing_time = 100U;
      public:
        startupIndexer()                       = default;
        ~startupIndexer()                      = default;
        startupIndexer(const startupIndexer &) = delete;
        startupIndexer &operator=(startupIndexer) = delete;
        auto start(std::shared_ptr<sys::Service> svc, std::string_view svc_name) -> void
        {
            collectStartupFiles();
            setupTimers(svc, svc_name);
        }
        static auto getFileType(std::string_view path) -> mimeType;

      private:
        // Collect startup files when service starts
        auto collectStartupFiles() -> void;
        // Setup timers for notification
        auto setupTimers(std::shared_ptr<sys::Service> svc, std::string_view svc_name) -> void;

      private:
        std::list<std::shared_ptr<msg::FileChangeMessage>> mMsgs;
        std::unique_ptr<sys::Timer> mIdxTimer;
    };
} // namespace service::detail
