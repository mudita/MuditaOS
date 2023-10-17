// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include "FS_Helper.hpp"
#include <Service/Service.hpp>
#include "FileOperations.hpp"

namespace sdesktop::endpoints
{

    class FilesystemEndpoint : public Endpoint
    {
        const std::unique_ptr<FS_Helper> helper;

      public:
        static auto createInstance(sys::Service *ownerServicePtr) -> std::unique_ptr<Endpoint>
        {
            return std::make_unique<FilesystemEndpoint>(ownerServicePtr, FileOperations::instance());
        }

        explicit FilesystemEndpoint(sys::Service *ownerServicePtr, FileOperations &fileOps)
            : Endpoint(ownerServicePtr), helper(std::make_unique<FS_Helper>(ownerServicePtr, fileOps))
        {}

        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
