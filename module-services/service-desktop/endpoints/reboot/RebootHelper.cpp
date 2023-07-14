// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Context.hpp>
#include <endpoints/reboot/RebootHelper.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <service-desktop/DesktopMessages.hpp>

#include <json11.hpp>

namespace sdesktop::endpoints
{
    auto RebootHelper::processPost(Context &context) -> ProcessResult
    {
        const auto rebootType = context.getBody()[json::reboot::rebootType].string_value();

        if (rebootType == json::reboot::msc) {
            sys::SystemManagerCommon::RebootMSC(owner);
            return {sent::no, ResponseContext{.status = http::Code::Accepted}};
        }
        else if (rebootType == json::reboot::reboot) {
            sys::SystemManagerCommon::Reboot(owner);
            return {sent::no, ResponseContext{.status = http::Code::Accepted}};
        }
        else if (rebootType == json::reboot::shutdown) {
            sys::SystemManagerCommon::RegularPowerDown(owner);
            return {sent::no, ResponseContext{.status = http::Code::Accepted}};
        }
        LOG_ERROR("Invalid request: %s", rebootType.c_str());
        return {sent::no, ResponseContext{.status = http::Code::BadRequest}};
    }
} // namespace sdesktop::endpoints
