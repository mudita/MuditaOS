// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <vfs.hpp>
#include "ServiceFileIndexer.hpp"
#include "messages/FileChangeMessage.hpp"
#include "Constants.hpp"
#include <Service/Bus.hpp>

namespace service
{

    // File indexer service constructor
    ServiceFileIndexer::ServiceFileIndexer(const std::string_view name) : sys::Service(std::string(name))
    {
        LOG_DEBUG("[%s] Initializing", std::string(name).c_str());
    }
    // File indexer service destructor
    ServiceFileIndexer::~ServiceFileIndexer()
    {}

    // When receive notification handler
    sys::Message_t ServiceFileIndexer::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
    {
        auto fcm = dynamic_cast<msg::FileChangeMessage *>(msg);
        if (fcm) {
            LOG_DEBUG("File notification handler received %s", fcm->newPath().c_str());
            return std::make_shared<sys::ResponseMessage>();
        }
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }

    // Initialize data notification handler
    sys::ReturnCodes ServiceFileIndexer::InitHandler()
    {
        vfs.registerNotificationHandler(
            [_this = shared_from_this()](std::string_view new_path, vfs::FsEvent event, std::string_view old_path) {
                namespace fs       = std::filesystem;
                const auto new_ext = fs::path(new_path).extension().string();
                auto msg           = std::make_shared<msg::FileChangeMessage>(new_path, event, old_path);
                sys::Bus::SendUnicast(msg, std::string(service::name::file_indexer), _this.get());
            });
        mStartupIndexer.start(shared_from_this(), service::name::file_indexer);
        return sys::ReturnCodes::Success;
    }

    // Deinitialization handler
    sys::ReturnCodes ServiceFileIndexer::DeinitHandler()
    {
        vfs.registerNotificationHandler(nullptr);
        return sys::ReturnCodes::Success;
    }

    // Power mode switch handler
    sys::ReturnCodes ServiceFileIndexer::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_DEBUG("Switch to power Mode %s", c_str(mode));
        return sys::ReturnCodes::Success;
    }
} // namespace service
