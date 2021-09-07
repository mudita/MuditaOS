// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <purefs/filesystem_paths.hpp>
#include <purefs/fs/inotify.hpp>
#include "ServiceFileIndexer.hpp"
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
            int err;
            if (inotify->flags && (purefs::fs::inotify_flags::close_write | purefs::fs::inotify_flags::move_dst)) {
                err = onUpdateOrCreate(inotify->name);
                if (err) {
                    LOG_ERROR("OnUpdateOrCreate failed with code %i", err);
                    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Failure);
                }
                else {
                    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Success);
                }
            }
            else if (inotify->flags && (purefs::fs::inotify_flags::del | purefs::fs::inotify_flags::move_src)) {
                err = onRemove(inotify->name);
                if (err) {
                    LOG_ERROR("On Remove failed with code %i", err);
                    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Failure);
                }
                else {
                    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Success);
                }
            }
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Success);
        }
        else {
            LOG_ERROR("Unhandled message");
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        }
    }

    // Initialize data notification handler
    sys::ReturnCodes ServiceFileIndexer::InitHandler()
    {
        mfsNotifier = purefs::fs::inotify_create(shared_from_this());
        if (!mfsNotifier) {
            LOG_ERROR("Unable to create inotify object");
            return sys::ReturnCodes::Failure;
        }
        // Wait for close, delete move to or from location
        const int err =
            mfsNotifier->add_watch(getMusicPath(),
                                   purefs::fs::inotify_flags::close_write | purefs::fs::inotify_flags::del |
                                       purefs::fs::inotify_flags::move_dst | purefs::fs::inotify_flags::move_src);
        if (err) {
            LOG_ERROR("Unable to create inotify watch errno: %i", err);
            return sys::ReturnCodes::Failure;
        }
        // Start the initial indexer
        mStartupIndexer.start(shared_from_this(), service::name::file_indexer);
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

    // On update or create content
    auto ServiceFileIndexer::onUpdateOrCreate(std::string_view path) -> int
    {
        LOG_DEBUG("**** TODO: On update or create indexer path: %s ****", std::string(path).c_str());
        return 0;
    }
    // On remove content
    auto ServiceFileIndexer::onRemove(std::string_view path) -> int
    {
        LOG_DEBUG("**** TODO: On update or create path: %s ****", std::string(path).c_str());
        return 0;
    }

} // namespace service
