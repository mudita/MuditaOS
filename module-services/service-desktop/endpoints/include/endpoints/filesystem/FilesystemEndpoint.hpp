// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include <Service/Service.hpp>
#include "FileOperations.hpp"

namespace sdesktop::endpoints
{

    class FilesystemEndpoint : public Endpoint
    {
      public:
        static auto createInstance(sys::Service *ownerServicePtr) -> std::unique_ptr<Endpoint>
        {
            return std::make_unique<FilesystemEndpoint>(ownerServicePtr, FileOperations::instance());
        }

        explicit FilesystemEndpoint(sys::Service *ownerServicePtr, FileOperations &fileOps)
            : Endpoint(ownerServicePtr), fileOps(fileOps)
        {}
        auto handle(Context &context) -> void override;
        auto runGet(Context &context) -> sys::ReturnCodes;
        auto runPost(Context &context) -> sys::ReturnCodes;
        auto runPut(Context &context) -> sys::ReturnCodes;
        auto getUpdates(Context &context) -> sys::ReturnCodes;

      private:
        auto startGetFile(Context &context) const -> sys::ReturnCodes;
        auto getFileChunk(Context &context) const -> sys::ReturnCodes;

        auto startSendFile(Context &context) const -> sys::ReturnCodes;
        auto sendFileChunk(Context &context) const -> sys::ReturnCodes;

        auto requestLogsFlush() const -> void;

        FileOperations &fileOps;
    };

} // namespace sdesktop::endpoints
