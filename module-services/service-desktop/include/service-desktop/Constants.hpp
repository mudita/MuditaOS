// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

namespace service::name
{
    inline constexpr auto service_desktop = "ServiceDesktop";
}
namespace sdesktop::paths
{

    inline constexpr auto updateFilename         = "update.tar";
    inline constexpr auto syncFilename           = "sync.tar";
    inline constexpr auto backupFilename         = "backup.tar";
    inline constexpr auto recoveryStatusFilename = "recovery_status.json";

} // namespace sdesktop::paths
