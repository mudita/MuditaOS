// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <vfs.hpp>
#include "ServiceFileIndexer.hpp"

namespace service
{

    ServiceFileIndexer::ServiceFileIndexer(const std::string_view name) : sys::Service(std::string(name))
    {
        LOG_DEBUG("[%s] Initializing", std::string(name).c_str());
    }

    ServiceFileIndexer::~ServiceFileIndexer()
    {}

    sys::Message_t ServiceFileIndexer::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
    {
        LOG_DEBUG("Receive handler data %p resp %p", msg, resp);
        return nullptr;
    }

    sys::ReturnCodes ServiceFileIndexer::InitHandler()
    {
        vfs.registerNotificationHandler([](std::string_view new_path, vfs::FsEvent event, std::string_view old_path) {
            LOG_DEBUG(
                "Filesystem change %s %i %s", std::string(new_path).c_str(), int(event), std::string(old_path).c_str());
        });
        return sys::ReturnCodes::Success;
    }
    sys::ReturnCodes ServiceFileIndexer::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }
    sys::ReturnCodes ServiceFileIndexer::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_DEBUG("Switch to power Mode %s", c_str(mode));
        return sys::ReturnCodes::Success;
    }
} // namespace service
