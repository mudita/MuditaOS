// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <Timers/TimerHandle.hpp>
#include <filesystem>

namespace service::detail
{
    class StartupIndexer
    {

      public:
        explicit StartupIndexer(const std::vector<std::string> &paths);
        ~StartupIndexer()                      = default;
        StartupIndexer(const StartupIndexer &) = delete;
        StartupIndexer &operator=(StartupIndexer) = delete;
        auto start(std::shared_ptr<sys::Service> svc, std::string_view svc_name) -> void;
        void reset();
        void stop();

      private:
        auto processEntry(std::shared_ptr<sys::Service> svc,
                          const std::filesystem::recursive_directory_iterator::value_type &entry) -> void;
        auto setupTimers(std::shared_ptr<sys::Service> svc, std::string_view svc_name) -> void;
        auto onTimerTimeout(std::shared_ptr<sys::Service> svc) -> void;

        std::vector<std::string>::const_iterator mTopDirIterator;
        std::filesystem::recursive_directory_iterator mSubDirIterator;
        sys::TimerHandle mIdxTimer;
        bool mForceStop{};

        const std::vector<std::string> directoriesToScan;
    };
} // namespace service::detail
