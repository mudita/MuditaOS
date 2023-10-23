// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-fileindexer/ServiceFileIndexer.hpp>

#include <service-fileindexer/ServiceFileIndexerName.hpp>

#include <log/log.hpp>
#include <purefs/filesystem_paths.hpp>

namespace
{
    inline constexpr auto fileIndexerServiceStackSize = 1024 * 5;
} // namespace

namespace service
{

    ServiceFileIndexer::ServiceFileIndexer(const std::vector<std::string> &paths)
        : sys::Service{service::name::file_indexer, "", fileIndexerServiceStackSize}, mStartupIndexer{paths}
    {
    }

    ServiceFileIndexer::~ServiceFileIndexer()
    {
    }

    sys::MessagePointer ServiceFileIndexer::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return sys::msgNotHandled();
    }

    // Initialize data notification handler
    sys::ReturnCodes ServiceFileIndexer::InitHandler()
    {
        if (mInotifyHandler.init(shared_from_this())) {
            mInotifyHandler.addWatch(purefs::dir::getUserMediaPath().c_str());

            // Start the initial indexer
            mStartupIndexer.start(shared_from_this(), service::name::file_indexer);
            return sys::ReturnCodes::Success;
        }

        LOG_INFO("Initialized");
        return sys::ReturnCodes::Failure;
    }

    sys::ReturnCodes ServiceFileIndexer::DeinitHandler()
    {
        LOG_INFO("Deinitialized");
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceFileIndexer::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_DEBUG("Switch to power Mode %s", c_str(mode));
        return sys::ReturnCodes::Success;
    }

    void ServiceFileIndexer::ProcessCloseReason(sys::CloseReason closeReason)
    {
        if (closeReason == sys::CloseReason::FactoryReset) {
            mStartupIndexer.reset();
        }
        else {
            mStartupIndexer.stop();
        }
    }
} // namespace service
