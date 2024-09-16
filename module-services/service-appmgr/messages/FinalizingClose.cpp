// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "FinalizingClose.hpp"

namespace app::manager
{
    FinalizingClose::FinalizingClose(const ApplicationName &sender)
        : BaseMessage(MessageType::APMFinalizingClose, sender)
    {}
} // namespace app::manager
