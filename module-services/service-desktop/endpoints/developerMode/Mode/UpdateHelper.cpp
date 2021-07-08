// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UpdateHelper.hpp"
#include <log.hpp>
#include <SystemManager/SystemManager.hpp>

namespace parserFSM
{
    void UpdateHelper::preProcess(http::Method method, Context &context)
    {
        LOG_INFO("In UpdateHelper - requesting %d", static_cast<int>(method));
    }

    auto UpdateHelper::processPost(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();
        if (body["reboot"] == true) {
            if (sys::SystemManager::RebootToUpdate(owner, sys::UpdateReason::Update)) {
                return {sent::no, endpoint::ResponseContext{.status = http::Code::OK}};
            }
            else {
                return {sent::no, endpoint::ResponseContext{.status = http::Code::InternalServerError}};
            }
        }
        return {sent::no, std::nullopt};
    }
} // namespace parserFSM
