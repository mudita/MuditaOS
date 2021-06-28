// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeviceInfoEndpointHelper.hpp"
#include <service-cellular/ServiceCellular.hpp>
#include <service-desktop/DesktopMessages.hpp>

namespace parserFSM
{
    class Context;
} // namespace parserFSM

using namespace parserFSM;

auto DeviceInfoEndpointHelper::processGet(Context &context) -> ProcessResult
{
    if (!requestDeviceInfo(owner)) {
        return {sent::no, endpoint::ResponseContext{.status = http::Code::NotAcceptable}};
    }
    else {
        return {sent::delayed, std::nullopt};
    }
}

bool DeviceInfoEndpointHelper::requestDeviceInfo(sys::Service *serv)
{
    auto event = std::make_unique<sdesktop::DeviceInfoRequestEvent>();
    auto msg   = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
    return serv->bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
}
