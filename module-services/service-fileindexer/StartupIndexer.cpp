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
#include <optional>

namespace service::detail
{
    namespace fs = std::filesystem;
    using namespace std::literals;
    using namespace std::chrono_literals;

    const auto lock_file_name        = purefs::dir::getUserDiskPath() / ".directory_is_indexed";
    constexpr auto indexing_interval = 50ms;
    constexpr auto start_delay       = 10000ms;

    bool isDirectoryFullyTraversed(const std::filesystem::recursive_directory_iterator &directory)
    {
        return directory == std::filesystem::recursive_directory_iterator();
    }

    bool createLockFile()
    {
        std::ofstream ofs(lock_file_name);
        ofs << time(nullptr);
        return ofs.good();
    }

    bool hasLockFile()
    {
        std::error_code ec;
        return fs::is_regular_file(lock_file_name, ec);
    }

    bool removeLockFile()
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

    std::optional<std::filesystem::recursive_directory_iterator> scanPath(std::vector<std::string>::const_iterator path)
    {
        std::error_code err;
        const auto mSubDirIterator = fs::recursive_directory_iterator(*path, err);
        if (err) {
            LOG_WARN("Directory '%s' not indexed, it does not exist", path->c_str());
            return std::nullopt;
        }
        return mSubDirIterator;
    }

    StartupIndexer::StartupIndexer(const std::vector<std::string> &paths) : directoriesToScan{paths}
    {}

    // Process single entry
    auto StartupIndexer::processEntry(std::shared_ptr<sys::Service> svc,
                                      const std::filesystem::recursive_directory_iterator::value_type &entry) -> void
    {
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

    auto StartupIndexer::onTimerTimeout(std::shared_ptr<sys::Service> svc) -> void
    {
        if (mForceStop) {
            return;
        }
        if (isDirectoryFullyTraversed(mSubDirIterator)) {
            if (mTopDirIterator == std::cend(directoriesToScan)) {
                createLockFile();
                LOG_INFO("Initial startup indexer - Finished ...");
                mIdxTimer.stop();
                return;
            }
            else {
                if (auto result = scanPath(mTopDirIterator)) {
                    mSubDirIterator = *result;
                }
                mTopDirIterator++;
            }
        }
        else {
            processEntry(svc, *mSubDirIterator);
            mSubDirIterator++;
        }

        mIdxTimer.restart(indexing_interval);
    }

    // Setup timers for notification
    auto StartupIndexer::setupTimers(std::shared_ptr<sys::Service> svc, std::string_view svc_name) -> void
    {
        mIdxTimer = sys::TimerFactory::createSingleShotTimer(
            svc.get(), svc_name.data(), start_delay, [this, svc](auto &) { onTimerTimeout(svc); });
        mIdxTimer.start();
    }

    // Start the initial file indexing
    auto StartupIndexer::start(std::shared_ptr<sys::Service> svc, std::string_view svc_name) -> void
    {
        if (!hasLockFile()) {
            LOG_INFO("Initial startup indexer - Started...");
            mTopDirIterator = std::begin(directoriesToScan);
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
} // namespace service::detail
