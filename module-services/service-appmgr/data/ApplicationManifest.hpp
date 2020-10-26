// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Types.hpp>

namespace app::manager
{
    struct ApplicationManifest
    {
        ApplicationName name;
        StartInBackground startInBackground{false};
        ActionFilter actions; ///< Actions the application can respond to.
    };
} // namespace app::manager
