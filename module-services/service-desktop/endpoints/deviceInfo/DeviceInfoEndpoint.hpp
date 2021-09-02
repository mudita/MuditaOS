// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include <parser/ParserUtils.hpp>

#include <Service/Service.hpp>

#include <string>

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

class DeviceInfoEndpoint : public parserFSM::Endpoint
{
  public:
    enum class DiagnosticsFileList
    {
        LOGS,
        CRASH_DUMPS
    };

    auto getSerialNumber() -> std::string;
    auto handleGet(parserFSM::Context &context) -> void;
    auto gatherListOfDiagnostics(parserFSM::Context &context, DiagnosticsFileList diagDataType) -> void;
    auto listDirectory(std::string path) -> std::vector<std::string>;
    auto fileListToJsonObject(const std::vector<std::string> &fileList) const -> json11::Json::object const;
    auto requestLogsFlush() const -> void;

  public:
    explicit DeviceInfoEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "DeviceInfoEndpoint";
    }
    auto handle(parserFSM::Context &context) -> void override;
    auto getDeviceInfo(parserFSM::Context &context) -> bool;
};
