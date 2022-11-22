// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Context.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <endpoints/update/UpdateHelper.hpp>
#include <log/log.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <purefs/filesystem_paths.hpp>

#include <filesystem>

namespace sdesktop::endpoints
{

    auto constexpr updatePackageFile = "update.tar";

    void UpdateHelper::preProcess(http::Method method, Context &context)
    {
        LOG_INFO("In UpdateHelper - requesting %d", static_cast<int>(method));
    }

    auto UpdateHelper::processPost(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();

        if (!(body[json::update] == true && body[json::reboot] == true)) {
            return {sent::no, ResponseContext{.status = http::Code::BadRequest}};
        }

        if (!std::filesystem::exists(purefs::dir::getUserDiskPath() / updatePackageFile)) {
            return {sent::no, ResponseContext{.status = http::Code::NotFound}};
        }

        if (sys::SystemManagerCommon::RebootToRecovery(owner, sys::RecoveryReason::Update)) {
            return {sent::no, ResponseContext{.status = http::Code::NoContent}};
        }

        return {sent::no, ResponseContext{.status = http::Code::InternalServerError}};
    }

    auto UpdateHelper::processPut(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();
        auto code        = http::Code::BadRequest;
        if (body[json::rebootMode] == json::usbMscMode) {
            code = sys::SystemManagerCommon::RebootToUsbMscMode(owner) ? http::Code::NoContent
                                                                       : http::Code::InternalServerError;
        }

        return {sent::no, ResponseContext{.status = code}};
    }

} // namespace sdesktop::endpoints
