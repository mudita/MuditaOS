﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include <Service/Service.hpp>

#include <string>
#include <tuple>
#include <filesystem>
#include <json11.hpp>

namespace sdesktop::endpoints
{
    class DeviceInfoEndpointCommon : public Endpoint
    {
      public:
        enum class DiagnosticFileType
        {
            Logs,
            CrashDumps
        };

        auto handleGet(Context &context) -> http::Code;
        auto gatherListOfDiagnostics(Context &context, DiagnosticFileType diagDataType) -> http::Code;
        auto parseDiagnosticFileType(const json11::Json &fileList) -> DiagnosticFileType;
        auto listDirectory(const std::string &path) -> std::vector<std::string>;
        auto fileListToJsonObject(const std::vector<std::string> &fileList) const -> json11::Json::object const;
        auto requestLogsFlush() const -> void;
        auto getStorageStats(const std::string &path) -> std::tuple<float, float>;
        auto getStorageInfo() -> std::tuple<float, float, float>;
        auto getMtpPath() const -> std::filesystem::path;

        explicit DeviceInfoEndpointCommon(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
        {
            debugName = "DeviceInfoEndpoint";
        }
        auto handle(Context &context) -> void override;
        virtual auto getDeviceInfo(Context &context) -> http::Code
        {
            return http::Code::BadRequest;
        };
    };

} // namespace sdesktop::endpoints
