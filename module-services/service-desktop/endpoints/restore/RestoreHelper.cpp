// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Context.hpp>
#include <endpoints/restore/RestoreHelper.hpp>
#include <service-desktop/DesktopMessages.hpp>

namespace sdesktop::endpoints
{
    auto RestoreHelper::processPost([[maybe_unused]] Context &context) -> ProcessResult
    {
        if (sys::SystemManagerCommon::RebootToUpdater(owner, sys::UpdaterReason::Restore)) {
            return {sent::no, ResponseContext{.status = http::Code::NoContent}};
        }

        return {sent::no, ResponseContext{.status = http::Code::InternalServerError}};
    }
} // namespace sdesktop::endpoints
