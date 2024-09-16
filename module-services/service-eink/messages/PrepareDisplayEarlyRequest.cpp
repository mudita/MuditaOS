// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "PrepareDisplayEarlyRequest.hpp"

namespace service::eink
{
    PrepareDisplayEarlyRequest::PrepareDisplayEarlyRequest(::gui::RefreshModes refreshMode) : refreshMode{refreshMode}
    {}

    auto PrepareDisplayEarlyRequest::getRefreshMode() const noexcept -> ::gui::RefreshModes
    {
        return refreshMode;
    }
} // namespace service::eink
