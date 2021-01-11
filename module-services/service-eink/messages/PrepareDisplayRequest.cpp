// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PrepareDisplayRequest.hpp"

namespace service::eink
{
    PrepareDisplayRequest::PrepareDisplayRequest(::gui::RefreshModes refreshMode) : refreshMode{refreshMode}
    {}

    auto PrepareDisplayRequest::getRefreshMode() const noexcept -> ::gui::RefreshModes
    {
        return refreshMode;
    }
} // namespace service::eink
