// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-fileindexer/InotifyHandler.hpp>

#include "Common.hpp"

#include <filesystem>
#include <log/log.hpp>
#include <module-db/queries/multimedia_files/QueryMultimediaFilesAdd.hpp>
#include <module-db/queries/multimedia_files/QueryMultimediaFilesRemove.hpp>
#include <purefs/fs/inotify_message.hpp>
#include <purefs/fs/inotify.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <tags_fetcher/TagsFetcher.hpp>
#include <Utils.hpp>

namespace service::detail
{
    InotifyHandler::~InotifyHandler()
    {
        for (const auto &path : monitoredPaths) {
            const auto err = mfsNotifier->rm_watch(path);
            if (err) {
                LOG_ERROR("Unable to remove watch errno: %i", err);
            }
        }
    }

    bool InotifyHandler::init(std::shared_ptr<sys::Service> service)
    {
        svc         = service;
        mfsNotifier = purefs::fs::inotify_create(svc);
        if (!mfsNotifier) {
            LOG_ERROR("Unable to create inotify object");
            return false;
        }
        registerMessageHandlers();
        return true;
    }

    void InotifyHandler::registerMessageHandlers()
    {
        if (svc == nullptr) {
            LOG_ERROR("svc is nullptr");
            return;
        }
        svc->connect(typeid(purefs::fs::message::inotify), [&](sys::Message *request) -> sys::MessagePointer {
            return handleInotifyMessage(static_cast<purefs::fs::message::inotify *>(request));
        });
    }

    bool InotifyHandler::addWatch(std::string_view monitoredPath)
    {
        if (mfsNotifier == nullptr) {
            LOG_ERROR("mfsNotifier is nullptr");
            return false;
        }
        // Wait for close, delete move to or from location
        const auto err =
            mfsNotifier->add_watch(monitoredPath,
                                   purefs::fs::inotify_flags::close_write | purefs::fs::inotify_flags::del |
                                       purefs::fs::inotify_flags::move_dst | purefs::fs::inotify_flags::move_src);
        if (err) {
            LOG_ERROR("Unable to create inotify watch errno: %i", err);
            return false;
        }
        monitoredPaths.emplace_back(monitoredPath);
        return true;
    }

    sys::MessagePointer InotifyHandler::handleInotifyMessage(purefs::fs::message::inotify *inotify)
    {
        if (inotify == nullptr)
            return sys::msgNotHandled();

        if (inotify->flags && (purefs::fs::inotify_flags::close_write | purefs::fs::inotify_flags::move_dst)) {
            onUpdateOrCreate(inotify->name);
        }
        else if (inotify->flags && (purefs::fs::inotify_flags::del | purefs::fs::inotify_flags::move_src)) {
            onRemove(inotify->name);
        }

        return sys::msgHandled();
    }

    namespace fs = std::filesystem;
    namespace
    {
        std::string getMimeType(const fs::path &path)
        {
            const auto extension = utils::stringToLowercase(path.extension());

            if (extension == ".mp3") {
                return "audio/mpeg";
            }
            if (extension == ".wav") {
                return "audio/wav";
            }
            if (extension == ".flac") {
                return "audio/flac";
            }
            return {};
        }

        std::optional<db::multimedia_files::MultimediaFilesRecord> CreateMultimediaFilesRecord(const fs::path &path)
        {
            std::error_code errorCode;
            auto fileSize = fs::file_size(path, errorCode);
            if (errorCode) {
                LOG_WARN("Can't get file size");
                return {};
            }
            auto mimeType = getMimeType(path);
            auto tags     = tags::fetcher::fetchTags(path);

            db::multimedia_files::MultimediaFilesRecord record{
                Record(DB_ID_NONE),
                .fileInfo = {.path = std::string(path), .mediaType = mimeType, .size = static_cast<size_t>(fileSize)},
                .tags =
                    {
                        .title = tags.title,
                        .album =
                            {
                                .artist = tags.artist,
                                .title  = tags.album,
                            },
                        .comment = tags.comment,
                        .genre   = tags.genre,
                        .year    = tags.year,
                        .track   = tags.track,
                    },
                .audioProperties = {.songLength = tags.total_duration_s,
                                    .bitrate    = tags.bitrate,
                                    .sampleRate = tags.sample_rate,
                                    .channels   = tags.num_channel}};

            return record;
        }
    } // namespace

    // On update or create content
    void InotifyHandler::onUpdateOrCreate(std::string_view path)
    {
        LOG_DEBUG("onUpdateOrCreate: %s", std::string(path).c_str());
        if (svc == nullptr) {
            LOG_ERROR("svc is nullptr");
            return;
        }

        const auto extension = utils::stringToLowercase(fs::path(path).extension());
        if (!isExtSupported(extension)) {
            LOG_WARN("Not supported extension - %s", extension.c_str());
            return;
        }

        auto record = CreateMultimediaFilesRecord(path);
        if (record.has_value()) {
            auto query = std::make_unique<db::multimedia_files::query::Add>(record.value());
            DBServiceAPI::GetQuery(svc.get(), db::Interface::Name::MultimediaFiles, std::move(query));
        }
        else {
            LOG_INFO("onUpdateOrCreate: skipped file");
        }
    }

    // On remove content
    void InotifyHandler::onRemove(std::string_view path)
    {
        LOG_DEBUG("onRemove path: %s", std::string(path).c_str());
        if (svc == nullptr) {
            LOG_ERROR("svc is nullptr");
            return;
        }

        const auto extension = utils::stringToLowercase(fs::path(path).extension());
        if (!isExtSupported(extension)) {
            LOG_WARN("Not supported extension - %s", extension.c_str());
            return;
        }

        auto query = std::make_unique<db::multimedia_files::query::RemoveByPath>(std::string(path));
        DBServiceAPI::GetQuery(svc.get(), db::Interface::Name::MultimediaFiles, std::move(query));
    }

} // namespace service::detail
