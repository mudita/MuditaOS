// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UpdateHelper.hpp"
#include <log.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <purefs/filesystem_paths.hpp>

#include <filesystem>

namespace parserFSM
{
    auto constexpr updatePackageFile = "update.tar";

    void UpdateHelper::preProcess(http::Method method, Context &context)
    {
        LOG_INFO("In UpdateHelper - requesting %d", static_cast<int>(method));
    }

    auto UpdateHelper::processPost(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();

        if (!(body["update"] == true && body["reboot"] == true)) {
            return {sent::no, endpoint::ResponseContext{.status = http::Code::BadRequest}};
        }

        if (!std::filesystem::exists(purefs::dir::getUserDiskPath() / updatePackageFile)) {
            return {sent::no, endpoint::ResponseContext{.status = http::Code::NotFound}};
        }

        if (sys::SystemManagerCommon::RebootToUpdate(owner, sys::UpdateReason::Update)) {
            return {sent::no, endpoint::ResponseContext{.status = http::Code::NoContent}};
        }

        return {sent::no, endpoint::ResponseContext{.status = http::Code::InternalServerError}};
    }
} // namespace parserFSM
