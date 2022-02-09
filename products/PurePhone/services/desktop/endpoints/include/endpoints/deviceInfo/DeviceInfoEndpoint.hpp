// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>

#include <Service/Service.hpp>

#include <string>
#include <tuple>

namespace sdesktop::endpoints
{
    class DeviceInfoEndpoint : public Endpoint
    {
      public:
        enum class DiagnosticFileType
        {
            Logs,
            CrashDumps
        };
        auto getSerialNumber() -> std::string;
        auto getCaseColour() -> std::string;
        auto handleGet(Context &context) -> void;
        auto gatherListOfDiagnostics(Context &context, DiagnosticFileType diagDataType) -> void;
        auto listDirectory(std::string path) -> std::vector<std::string>;
        auto fileListToJsonObject(const std::vector<std::string> &fileList) const -> json11::Json::object const;
        auto requestLogsFlush() const -> void;
        auto getStorageStats(const std::string &path) -> std::tuple<long, long>;
        auto getDeviceToken() -> std::string;

        static constexpr auto OS_RESERVED_SPACE_IN_MB = (1024LU);

      public:
        explicit DeviceInfoEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
        {
            debugName = "DeviceInfoEndpoint";
        }
        auto handle(Context &context) -> void override;
        auto getDeviceInfo(Context &context) -> bool;
    };

} // namespace sdesktop::endpoints
