// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>

#include "Constants.hpp"
#include "StartupIndexer.hpp"

namespace purefs::fs
{
    class inotify;
}

namespace service
{

    class ServiceFileIndexer final : public sys::Service
    {
      public:
        ServiceFileIndexer(const std::string_view name = service::name::file_indexer);
        virtual ~ServiceFileIndexer()                  = default;
        ServiceFileIndexer(const ServiceFileIndexer &) = delete;
        ServiceFileIndexer &operator=(const ServiceFileIndexer &) = delete;
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

      private:
        // On update or create content
        auto onUpdateOrCreate(std::string_view path) -> int;
        // On remove content
        auto onRemove(std::string_view path) -> int;

      private:
        std::shared_ptr<purefs::fs::inotify> mfsNotifier;
        detail::StartupIndexer mStartupIndexer;
    };

}; // namespace service

namespace sys
{
    template <> struct ManifestTraits<service::ServiceFileIndexer>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name = service::name::file_indexer;
            return manifest;
        }
    };
} // namespace sys
