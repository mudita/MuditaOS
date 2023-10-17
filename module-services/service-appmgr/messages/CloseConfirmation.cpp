// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CloseConfirmation.hpp"

namespace app::manager
{
    CloseConfirmation::CloseConfirmation(const ApplicationName &sender)
        : BaseMessage(MessageType::APMConfirmClose, sender)
    {}
} // namespace app::manager
