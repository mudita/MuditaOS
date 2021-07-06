// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <purefs/filesystem_paths.hpp>
#include <purefs/fs/inotify.hpp>
#include "ServiceFileIndexer.hpp"
#include <fileref.h>
#include <tag.h>
#include "notesIndexer.hpp"
#include "Constants.hpp"
#include <purefs/fs/inotify_message.hpp>

namespace
{
    inline auto getMusicPath()
    {
        return purefs::createPath(purefs::dir::getUserDiskPath(), "music").string();
    }
} // namespace

namespace service
{

    ServiceFileIndexer::ServiceFileIndexer(const std::string_view name) : sys::Service(std::string(name))
    {
        LOG_DEBUG("[%s] Initializing", std::string(name).c_str());
    }

    // When receive notification handler
    sys::MessagePointer ServiceFileIndexer::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
    {
        auto inotify = dynamic_cast<purefs::fs::message::inotify *>(msg);
        if (inotify) {
            LOG_ERROR("Inotify event %s %08x", inotify->name.c_str(), int(inotify->flags));
        }
        else {
            LOG_ERROR("Not a inotify message");
        }

#if 0
        auto fcm = dynamic_cast<msg::FileChangeMessage *>(msg);
        if (fcm) {
            switch (fcm->event()) {
            case msg::FileChangeMessage::evt_t::modified: {
                switch (detail::StartupIndexer::getFileType(fcm->newPath())) {
                case detail::mimeType::audio:
                    onAudioContentChanged(fcm->newPath());
                    break;
                case detail::mimeType::text:
                    onTextContentChanged(fcm->newPath());
                    break;
                default:
                    LOG_INFO("Skip indexing file");
                    break;
                }
            } break;
            case msg::FileChangeMessage::evt_t::renamed:
                onRenameFile(fcm->oldPath(), fcm->newPath());
                break;
            case msg::FileChangeMessage::evt_t::deleted:
                onDeleteFile(fcm->newPath());
                break;
            case msg::FileChangeMessage::evt_t::initialized:
                break;
            }
            return std::make_shared<sys::ResponseMessage>();
        }
#endif
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }

    // Initialize data notification handler
    sys::ReturnCodes ServiceFileIndexer::InitHandler()
    {
        /*
        mStartupIndexer.start(shared_from_this(), service::name::file_indexer);
        */

        mfsNotifier = purefs::fs::inotify_create(shared_from_this());
        if (!mfsNotifier) {
            LOG_ERROR("Unable to create inotify object");
            return sys::ReturnCodes::Failure;
        }
        const int err = mfsNotifier->add_watch(getMusicPath(), purefs::fs::inotify_flags::close_write);
        if (err) {
            LOG_ERROR("Unable to create inotify watch errno: %i", err);
            return sys::ReturnCodes::Failure;
        }
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceFileIndexer::DeinitHandler()
    {
        const int err = mfsNotifier->rm_watch(getMusicPath());
        if (err) {
            LOG_ERROR("Unable to remove watch errno: %i", err);
            return sys::ReturnCodes::Failure;
        }
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceFileIndexer::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_DEBUG("Switch to power Mode %s", c_str(mode));
        return sys::ReturnCodes::Success;
    }

    // When file is changed update db only
    auto ServiceFileIndexer::onDeleteFile(std::string_view path) -> void
    {
        LOG_DEBUG("File deleted");
    }
    // When file is renamed
    auto ServiceFileIndexer::onRenameFile(std::string_view oldPath, std::string_view newPath) -> void
    {
        LOG_DEBUG("File renamed");
    }
    // On audio file content change
    auto ServiceFileIndexer::onAudioContentChanged(std::string_view path) -> void
    {
        LOG_DEBUG("Audio content index...");
        TagLib::FileRef fref(std::string(path).c_str());
        if (!fref.isNull() && fref.tag()) {
            const auto tag = fref.tag();
            LOG_DEBUG(">>>>> title %s", tag->title().toCString());
            LOG_DEBUG(">>>>> artist %s", tag->artist().toCString());
            LOG_DEBUG(">>>> album %s", tag->album().toCString());
            LOG_DEBUG(">>>>> year %i", tag->year());
            LOG_DEBUG(">>>>> comment %s", tag->comment().toCString());
            LOG_DEBUG(">>>> track %u", tag->track());
            LOG_DEBUG(">>>> genre %s", tag->genre().toCString());
        }
        if (!fref.isNull() && fref.audioProperties()) {
            const auto prop = fref.audioProperties();
            int seconds     = prop->length() % 60;
            int minutes     = (prop->length() - seconds) / 60;
            LOG_DEBUG(">>>>> bitrate %i", prop->bitrate());
            LOG_DEBUG(">>>>> samplerate %i", prop->sampleRate());
            LOG_DEBUG(">>>>> channels %i", prop->channels());
            LOG_DEBUG(">>>>> length %02i:%02i", minutes, seconds);
        }
    }
    // On text file content change
    auto ServiceFileIndexer::onTextContentChanged(std::string_view path) -> void
    {
        LOG_DEBUG("Text content index...");
        detail::notesIndexer noteInfo(path);
        LOG_DEBUG("Words %zu Lines %zu Chars %zu Size %zu",
                  noteInfo.getWords(),
                  noteInfo.getLines(),
                  noteInfo.getChars(),
                  noteInfo.getFileSize());
    }
} // namespace service
