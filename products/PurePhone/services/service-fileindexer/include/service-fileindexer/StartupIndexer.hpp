// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <module-sys/Timers/TimerHandle.hpp>
#include <filesystem>

namespace service::detail
{
    class StartupIndexer
    {

      public:
        StartupIndexer()                       = default;
        ~StartupIndexer()                      = default;
        StartupIndexer(const StartupIndexer &) = delete;
        StartupIndexer &operator=(StartupIndexer) = delete;
        auto start(std::shared_ptr<sys::Service> svc, std::string_view svc_name) -> void;

      private:
        // Process single entry
        static auto processEntry(std::shared_ptr<sys::Service> svc,
                                 const std::filesystem::recursive_directory_iterator::value_type &entry) -> void;
        // Setup timers for notification
        auto setupTimers(std::shared_ptr<sys::Service> svc, std::string_view svc_name) -> void;
        // On timer timeout
        auto onTimerTimeout(std::shared_ptr<sys::Service> svc) -> void;
        // Create lock file
        static auto createLockFile() -> bool;
        //  Check if lock file exists
        static auto hasLockFile() -> bool;

      private:
        std::vector<std::string>::const_iterator mTopDirIterator;
        std::filesystem::recursive_directory_iterator mSubDirIterator;
        sys::TimerHandle mIdxTimer;
        bool mStarted{};
    };
} // namespace service::detail
