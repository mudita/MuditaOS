﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FactoryReset.hpp"
#include <SystemManager/SystemManager.hpp>
#include <log/log.hpp>
#include <purefs/filesystem_paths.hpp>
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

    static bool CopyFile(std::string sourcefile, std::string targetfile);

    static int recurseDepth            = 0;
    static const int max_recurse_depth = 120; /* 120 is just an arbitrary value of max number of recursive calls.
                                               * If more then error is assumed, not the real depth of directories."
                                               */
    static const int max_filepath_length = PATH_MAX;

    bool Run(sys::Service *ownerService)
    {
        LOG_INFO("FactoryReset: restoring factory state started...");

        recurseDepth             = 0;
        const auto factoryOSPath = purefs::dir::getFactoryOSPath();

        if (std::filesystem::is_directory(factoryOSPath.c_str()) && std::filesystem::is_empty(factoryOSPath.c_str())) {
            LOG_ERROR("FactoryReset: restoring factory state aborted");
            LOG_ERROR("FactoryReset: directory %s seems empty.", factoryOSPath.c_str());
            return false;
        }

        if (ownerService != nullptr) {
            LOG_INFO("FactoryReset: closing ServiceDB...");
            std::string dbServiceName = service::name::db;
            sys::SystemManager::DestroyService(dbServiceName, ownerService);
        }

        if (DeleteDirContent(purefs::dir::getRootDiskPath()) != true) {
            LOG_ERROR("FactoryReset: restoring factory state aborted");
            return false;
        }

        if (CopyDirContent(factoryOSPath, purefs::dir::getRootDiskPath()) != true) {
            LOG_ERROR("FactoryReset: restoring factory state aborted");
            return false;
        }

        LOG_INFO("FactoryReset: restoring factory state finished, rebooting...");
        sys::SystemManager::Reboot(ownerService);
        return true;
    }

    bool DeleteDirContent(std::string dir)
    {
        for (auto &direntry : std::filesystem::directory_iterator(dir.c_str())) {
            if ((direntry.path().string().compare(".") != 0) && (direntry.path().string().compare("..") != 0) &&
                (direntry.path().string().compare("...") != 0)) {

                std::string delpath = dir;
                delpath += "/";
                delpath += direntry.path().string().c_str();

                if (std::filesystem::is_directory(direntry)) {
                    if (direntry.path().string().compare(purefs::dir::getFactoryOSPath()) != 0) {
                        LOG_INFO("FactoryReset: recursively deleting dir %s...", delpath.c_str());
                        try {
                            std::filesystem::remove_all(delpath.c_str());
                        }
                        catch (const std::filesystem::filesystem_error &e) {
                            LOG_ERROR("FactoryReset: error deleting dir %s, aborting...", delpath.c_str());
                            return false;
                        }
                    }
                }
                else {
                    LOG_INFO("FactoryReset: deleting file %s...", delpath.c_str());
                    if (std::filesystem::remove(delpath.c_str())) {
                        LOG_ERROR("FactoryReset: error deleting file %s, aborting...", delpath.c_str());
                        return false;
                    }
                }
            }
        }

        return true;
    }

    bool CopyDirContent(std::string sourcedir, std::string targetdir)
    {
        if (recurseDepth >= max_recurse_depth) {
            LOG_ERROR("FactoryReset: recurse level %d (too high), error assumed, skipping restore of dir %s",
                      recurseDepth,
                      sourcedir.c_str());
            return false;
        }

        const auto factoryOSPath = purefs::dir::getFactoryOSPath();

        for (auto &direntry : std::filesystem::directory_iterator(sourcedir.c_str())) {
            if ((direntry.path().string().compare(".") == 0) || (direntry.path().string().compare("..") == 0) ||
                (direntry.path().string().compare("...") == 0)) {
                continue;
            }

            std::string sourcepath = sourcedir;
            sourcepath += "/";
            sourcepath += direntry.path().string().c_str();

            std::string targetpath = targetdir;
            targetpath += "/";
            targetpath += direntry.path().string().c_str();

            if ((sourcepath.size() >= max_filepath_length) || (targetpath.size() >= max_filepath_length)) {
                LOG_ERROR("FactoryReset: path length (source or target) exceeds system limit of %d",
                          max_filepath_length);
                LOG_ERROR("FactoryReset: skipping restore of dir %s into %s", sourcepath.c_str(), targetpath.c_str());
                return false;
            }

            if (std::filesystem::is_directory(direntry)) {
                if (targetpath.compare(factoryOSPath.c_str()) == 0) {
                    continue;
                }

                LOG_INFO("FactoryReset: restoring dir  %s into %s...", sourcepath.c_str(), targetpath.c_str());

                try {
                    if (std::filesystem::create_directory(targetpath.c_str())) {
                        LOG_ERROR("FactoryReset: create dir %s failed", targetpath.c_str());
                        return false;
                    }
                }
                catch (const std::filesystem::filesystem_error &err) {
                    LOG_FATAL("Exception while creating dir %s", targetpath.c_str());
                    return false;
                }

                recurseDepth++;

                if (CopyDirContent(sourcepath, targetpath) != true) {
                    recurseDepth--;
                    return false;
                }

                recurseDepth--;
            }
            else {
                LOG_INFO("FactoryReset: restoring file %s into %s...", sourcepath.c_str(), targetpath.c_str());

                if (CopyFile(sourcepath, targetpath) != true) {
                    return false;
                }
            }
        }

        return true;
    }

    static bool CopyFile(std::string sourcefile, std::string targetfile)
    {
        bool ret  = true;
        auto lamb = [](std::FILE *stream) { std::fclose(stream); };

        std::unique_ptr<std::FILE, decltype(lamb)> sf(std::fopen(sourcefile.c_str(), "r"), lamb);
        std::unique_ptr<std::FILE, decltype(lamb)> tf(std::fopen(targetfile.c_str(), "w"), lamb);

        if ((sf.get() != nullptr) && (tf.get() != nullptr)) {
            std::unique_ptr<unsigned char[]> buffer(new unsigned char[copy_buf]);

            if (buffer.get() != nullptr) {
                uint32_t loopcount = (std::filesystem::file_size(sourcefile) / copy_buf) + 1u;
                uint32_t readsize  = copy_buf;

                for (uint32_t i = 0u; i < loopcount; i++) {
                    if (i + 1u == loopcount) {
                        readsize = std::filesystem::file_size(sourcefile) % copy_buf;
                    }

                    if (std::fread(buffer.get(), 1, readsize, sf.get()) != readsize) {
                        LOG_ERROR("FactoryReset: read from sourcefile %s failed", sourcefile.c_str());
                        ret = false;
                        break;
                    }

                    if (std::fwrite(buffer.get(), 1, readsize, tf.get()) != readsize) {
                        LOG_ERROR("FactoryReset: write to targetfile %s failed", targetfile.c_str());
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
