// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "SwitchConfirmation.hpp"

namespace app::manager
{
    SwitchConfirmation::SwitchConfirmation(const ApplicationName &sender)
        : BaseMessage(MessageType::APMConfirmSwitch, sender)
    {}
} // namespace app::manager
