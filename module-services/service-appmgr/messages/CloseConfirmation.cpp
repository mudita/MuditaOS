// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "CloseConfirmation.hpp"

namespace app::manager
{
    CloseConfirmation::CloseConfirmation(const ApplicationName &sender)
        : BaseMessage(MessageType::APMConfirmClose, sender)
    {}
} // namespace app::manager
