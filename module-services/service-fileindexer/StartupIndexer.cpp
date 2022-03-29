// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Common.hpp"
#include <service-fileindexer/StartupIndexer.hpp>
#include <service-fileindexer/Constants.hpp>

#include <Timers/TimerFactory.hpp>
#include <purefs/filesystem_paths.hpp>
#include <purefs/fs/inotify_message.hpp>
#include <log/log.hpp>

#include <filesystem>
#include <fstream>

namespace service::detail
{
    namespace fs = std::filesystem;
    namespace
    {
        using namespace std::string_literals;
        // Lock file name
        const auto lock_file_name = purefs::dir::getUserDiskPath() / ".directory_is_indexed";
        // Time for indexing first unit
        constexpr auto timer_indexing_delay = 400;
        // Time for initial delay after start
        constexpr auto timer_run_delay = 10000;
    } // namespace

    StartupIndexer::StartupIndexer(const std::vector<std::string> &paths) : start_dirs{paths}
    {}

    // Process single entry
    auto StartupIndexer::processEntry(std::shared_ptr<sys::Service> svc,
                                      const std::filesystem::recursive_directory_iterator::value_type &entry) -> void
    {
        using namespace std::string_view_literals;
        if (fs::is_regular_file(entry)) {
            auto ext = fs::path(entry).extension();
            if (!isExtSupported(ext)) {
                LOG_WARN("Not supported ext - %s", ext.c_str());
                return;
            }

            const auto abspath = fs::absolute(entry).string();
            const auto inotifyMsg =
                std::make_shared<purefs::fs::message::inotify>(purefs::fs::inotify_flags::close_write, abspath, ""sv);
            svc->bus.sendUnicast(inotifyMsg, std::string(service::name::file_indexer));
        }
    }

    // On timer timeout
    auto StartupIndexer::onTimerTimeout(std::shared_ptr<sys::Service> svc) -> void
    {
        if (mForceStop) {
            return;
        }
        if (!mStarted) {
            mIdxTimer.restart(std::chrono::milliseconds{timer_indexing_delay});
            mStarted = true;
        }
        if (mSubDirIterator == std::filesystem::recursive_directory_iterator()) {
            if (mTopDirIterator == std::cend(start_dirs)) {
                createLockFile();
                LOG_INFO("Initial startup indexer - Finished ...");
                mIdxTimer.stop();
            }
            else {
                mSubDirIterator = fs::recursive_directory_iterator(*mTopDirIterator);
                mTopDirIterator++;
            }
        }
        else {
            processEntry(svc, *mSubDirIterator);
            mSubDirIterator++;
        }
    }

    // Setup timers for notification
    auto StartupIndexer::setupTimers(std::shared_ptr<sys::Service> svc, std::string_view svc_name) -> void
    {
        mIdxTimer = sys::TimerFactory::createPeriodicTimer(svc.get(),
                                                           "file_indexing",
                                                           std::chrono::milliseconds{timer_run_delay},
                                                           [this, svc](sys::Timer &) { onTimerTimeout(svc); });
        mIdxTimer.start();
    }

    // Start the initial file indexing
    auto StartupIndexer::start(std::shared_ptr<sys::Service> svc, std::string_view svc_name) -> void
    {
        if (!hasLockFile()) {
            LOG_INFO("Initial startup indexer - Started...");
            mTopDirIterator = std::begin(start_dirs);
            setupTimers(svc, svc_name);
            mForceStop = false;
        }
        else {
            LOG_INFO("Initial startup indexer - Not needed...");
        }
    }

    void StartupIndexer::stop()
    {
        mForceStop = true;
        mIdxTimer.stop();
    }

    void StartupIndexer::reset()
    {
        stop();
        removeLockFile();
    }

    auto StartupIndexer::createLockFile() -> bool
    {
        std::ofstream ofs(lock_file_name);
        ofs << time(nullptr);
        return ofs.good();
    }

    auto StartupIndexer::hasLockFile() -> bool
    {
        std::error_code ec;
        return fs::is_regular_file(lock_file_name, ec);
    }

    auto StartupIndexer::removeLockFile() -> bool
    {
        if (hasLockFile()) {
            std::error_code ec;
            if (!remove(lock_file_name, ec)) {
                LOG_ERROR("Failed to remove lock file, error: %d", ec.value());
                return false;
            }
        }
        return true;
    }
} // namespace service::detail
