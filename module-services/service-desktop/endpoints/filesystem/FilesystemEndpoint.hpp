// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-services/service-desktop/endpoints/Endpoint.hpp>
#include "Service/Service.hpp"
#include "FileOperations.hpp"

class FilesystemEndpoint : public parserFSM::Endpoint
{
  public:
    static auto createInstance(sys::Service *ownerServicePtr) -> std::unique_ptr<parserFSM::Endpoint>
    {
        return std::make_unique<FilesystemEndpoint>(ownerServicePtr, FileOperations::instance());
    }

    explicit FilesystemEndpoint(sys::Service *ownerServicePtr, FileOperations &fileOps)
        : Endpoint(ownerServicePtr), fileOps(fileOps)
    {}
    auto handle(parserFSM::Context &context) -> void override;
    auto runGet(parserFSM::Context &context) -> sys::ReturnCodes;
    auto runPost(parserFSM::Context &context) -> sys::ReturnCodes;
    auto runPut(parserFSM::Context &context) -> sys::ReturnCodes;
    auto getUpdates(parserFSM::Context &context) -> sys::ReturnCodes;

  private:
    auto startGetFile(parserFSM::Context &context) const -> sys::ReturnCodes;
    auto getFileChunk(parserFSM::Context &context) const -> sys::ReturnCodes;

    auto startSendFile(parserFSM::Context &context) const -> sys::ReturnCodes;
    auto sendFileChunk(parserFSM::Context &context) const -> sys::ReturnCodes;

    auto requestLogsFlush() const -> void;

    FileOperations &fileOps;
};
