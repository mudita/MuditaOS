// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>

#include "ServiceFileIndexerDependencies.hpp"
#include "ServiceFileIndexerName.hpp"
#include "StartupIndexer.hpp"
#include "InotifyHandler.hpp"

namespace service
{

    class ServiceFileIndexer final : public sys::Service
    {
      public:
        explicit ServiceFileIndexer(const std::vector<std::string> &paths);
        ~ServiceFileIndexer() override;
        ServiceFileIndexer(const ServiceFileIndexer &) = delete;
        ServiceFileIndexer &operator=(const ServiceFileIndexer &) = delete;
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
        void ProcessCloseReason(sys::CloseReason closeReason) override;

      private:
        detail::InotifyHandler mInotifyHandler;
        detail::StartupIndexer mStartupIndexer;
    };

}; // namespace service

namespace sys
{
    template <>
    struct ManifestTraits<service::ServiceFileIndexer>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::file_indexer;
            manifest.dependencies = sys::dependencies::getDependenciesFor<service::ServiceFileIndexer>();
            return manifest;
        }
    };
} // namespace sys
