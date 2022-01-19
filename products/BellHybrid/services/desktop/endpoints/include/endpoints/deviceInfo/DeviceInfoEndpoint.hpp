// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>

#include <Service/Service.hpp>

#include <string>

namespace sdesktop::endpoints
{

    class DeviceInfoEndpoint : public Endpoint
    {
        auto getSerialNumber() -> std::string;
        auto getStorageStats(const std::string &path) -> std::tuple<long, long>;

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
