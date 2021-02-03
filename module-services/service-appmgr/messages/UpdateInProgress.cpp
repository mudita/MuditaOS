// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/messages/UpdateInProgress.hpp>

namespace app::manager
{
    UpdateInProgress::UpdateInProgress(const ApplicationName &sender)
        : BaseMessage(MessageType::APMDelayedClose, sender)
    {}

} // namespace app::manager
