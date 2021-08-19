// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FactoryReset.hpp"
#include <SystemManager/SystemManagerCommon.hpp>
#include <log.hpp>
#include <service-db/DBServiceName.hpp>
#include <Utils.hpp>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>
#include <purefs/filesystem_paths.hpp>
#include <purefs/fs/filesystem.hpp>
#include <limits.h>

namespace sys
{
    class Service;
} // namespace sys

namespace FactoryReset
{
    namespace
    {
        inline constexpr auto copy_buf = 8192 * 4;
    } // namespace

    static bool CopyFile(const std::string &sourcefile, const std::string &targetfile);

    static int recurseDepth            = 0;
    static const int max_recurse_depth = 120; /* 120 is just an arbitrary value of max number of recursive calls.
                                               * If more then error is assumed, not the real depth of directories."
                                               */
    static const int max_filepath_length = PATH_MAX;

    bool Run(sys::Service *ownerService)
    {
        LOG_INFO("Restoring factory state started...");

        recurseDepth          = 0;
        const auto userOSPath = purefs::dir::getUserDiskPath();

        if (std::filesystem::is_directory(userOSPath.c_str()) && std::filesystem::is_empty(userOSPath.c_str())) {
            LOG_ERROR("Restoring factory state aborted");
            LOG_ERROR("Directory %s seems empty.", userOSPath.c_str());
            return false;
        }

        if (ownerService != nullptr) {
            LOG_INFO("Closing ServiceDB...");
            std::string dbServiceName = service::name::db;
            sys::SystemManagerCommon::DestroySystemService(dbServiceName, ownerService);
        }

        DeleteSelectedUserFiles(userOSPath);

        LOG_INFO("Rebooting...");
        sys::SystemManagerCommon::Reboot(ownerService);
        return true;
    }

    bool DeleteSelectedUserFiles(const std::filesystem::path &userOSPath)
    {
        bool returnStatus                        = true;
        std::vector<std::string> selectedFileExt = {".db", ".db-journal", ".db-wal"};

        LOG_INFO("Delete DB files which will be recreated with factory content after reboot:");
        for (const auto &f : std::filesystem::directory_iterator(userOSPath.c_str())) {
            for (const auto &ext : selectedFileExt) {
                if (f.path().extension() == ext) {
                    auto removeStatus = std::filesystem::remove(f.path());
                    if (!removeStatus) {
                        LOG_ERROR("Error deleting file %s, aborting...", f.path().c_str());
                        returnStatus = false;
                    }
                    else {
                        LOG_INFO("%s deleted.", f.path().c_str());
                    }
                    break;
                }
            }
        }
        return returnStatus;
    }

    bool DeleteDirContent(const std::string &dir)
    {
        for (auto &direntry : std::filesystem::directory_iterator(dir.c_str())) {
            if (!((direntry.path().string() != ".") && (direntry.path().string() != "..") &&
                  (direntry.path().string() != "..."))) {
                continue;
            }
            std::string delpath = dir;
            delpath += "/";
            delpath += direntry.path().string();

            if (std::filesystem::is_directory(direntry)) {
                if (direntry.path().string() != purefs::dir::getFactoryOSPath()) {
                    LOG_INFO("FactoryReset: recursively deleting dir...");
                    try {
                        std::filesystem::remove_all(delpath.c_str());
                    }
                    catch (const std::filesystem::filesystem_error &e) {
                        LOG_ERROR("FactoryReset: error deleting dir, aborting...");
                        return false;
                    }
                }
            }
            else {
                LOG_INFO("FactoryReset: deleting file...");
                if (std::filesystem::remove(delpath.c_str())) {
                    LOG_ERROR("FactoryReset: error deleting file, aborting...");
                    return false;
                }
            }
        }

        return true;
    }

    bool CopyDirContent(const std::string &sourcedir, const std::string &targetdir)
    {
        if (recurseDepth >= max_recurse_depth) {
            LOG_ERROR("FactoryReset: recurse level %d (too high), error assumed, skipping restore of dir",
                      recurseDepth);
            return false;
        }

        const auto factoryOSPath = purefs::dir::getFactoryOSPath();

        for (auto &direntry : std::filesystem::directory_iterator(sourcedir.c_str())) {
            if ((direntry.path().string() == ".") || (direntry.path().string() == "..") ||
                (direntry.path().string() == "...")) {
                continue;
            }

            std::string sourcepath = sourcedir;
            sourcepath += "/";
            sourcepath += direntry.path().string();

            std::string targetpath = targetdir;
            targetpath += "/";
            targetpath += direntry.path().string();

            if ((sourcepath.size() >= max_filepath_length) || (targetpath.size() >= max_filepath_length)) {
                LOG_ERROR("FactoryReset: path length (source or target) exceeds system limit of %d",
                          max_filepath_length);
                LOG_ERROR("FactoryReset: skipping restore of directory");
                return false;
            }

            if (std::filesystem::is_directory(direntry)) {
                if (targetpath == factoryOSPath) {
                    continue;
                }

                LOG_INFO("FactoryReset: restoring directory");

                try {
                    if (std::filesystem::create_directory(targetpath.c_str())) {
                        LOG_ERROR("FactoryReset: create dir failed");
                        return false;
                    }
                }
                catch (const std::filesystem::filesystem_error &err) {
                    LOG_FATAL("Exception while creating dir");
                    return false;
                }

                recurseDepth++;

                if (!CopyDirContent(sourcepath, targetpath)) {
                    recurseDepth--;
                    return false;
                }

                recurseDepth--;
            }
            else {
                LOG_INFO("FactoryReset: restoring file");

                if (!CopyFile(sourcepath, targetpath)) {
                    return false;
                }
            }
        }

        return true;
    }

    static bool CopyFile(const std::string &sourcefile, const std::string &targetfile)
    {
        bool ret  = true;
        auto lamb = [](std::FILE *stream) { std::fclose(stream); };

        std::unique_ptr<std::FILE, decltype(lamb)> sf(std::fopen(sourcefile.c_str(), "r"), lamb);
        std::unique_ptr<std::FILE, decltype(lamb)> tf(std::fopen(targetfile.c_str(), "w"), lamb);

        if (sf && tf) {
            std::unique_ptr<unsigned char[]> buffer(new unsigned char[copy_buf]);

            if (buffer) {
                uint32_t loopcount = (std::filesystem::file_size(sourcefile) / copy_buf) + 1u;
                uint32_t readsize  = copy_buf;

                for (uint32_t i = 0u; i < loopcount; i++) {
                    if (i + 1u == loopcount) {
                        readsize = std::filesystem::file_size(sourcefile) % copy_buf;
                    }

                    if (std::fread(buffer.get(), 1, readsize, sf.get()) != readsize) {
                        LOG_ERROR("FactoryReset: read from sourcefile failed");
                        ret = false;
                        break;
                    }

                    if (std::fwrite(buffer.get(), 1, readsize, tf.get()) != readsize) {
                        LOG_ERROR("FactoryReset: write to targetfile failed");
                        ret = false;
                        break;
                    }
                }
            }
            else {
                LOG_ERROR("FactoryReset: unable to open copy buffer");
                ret = false;
            }
        }
        else {
            LOG_ERROR("FactoryReset: unable to open source or target file");
            ret = false;
        }

        return ret;
    }
} // namespace FactoryReset
