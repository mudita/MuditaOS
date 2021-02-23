// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/messages/SetOsUpdateVersion.hpp>

namespace app::manager
{
    SetOsUpdateVersion::SetOsUpdateVersion(const ApplicationName &sender,
                                           const std::string &osUpdateVer,
                                           const std::string &osCurrentVer)
        : BaseMessage(MessageType::UpdateOS, sender), osUpdateVer(osUpdateVer), osCurrentVer(osCurrentVer)
    {}

} // namespace app::manager
