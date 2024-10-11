// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <endpoints/Context.hpp>
#include <endpoints/restore/RestoreHelper.hpp>
#include <service-desktop/DesktopMessages.hpp>

namespace sdesktop::endpoints
{
    auto RestoreHelper::processPost([[maybe_unused]] Context &context) -> ProcessResult
    {
        if (sys::SystemManagerCommon::RebootToRecovery(owner, sys::RecoveryReason::Restore)) {
            return {Sent::No, ResponseContext{.status = http::Code::NoContent}};
        }

        return {Sent::No, ResponseContext{.status = http::Code::InternalServerError}};
    }
} // namespace sdesktop::endpoints
