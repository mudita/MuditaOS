// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FinalizingClose.hpp"

namespace app::manager
{
    FinalizingClose::FinalizingClose(const ApplicationName &sender)
        : BaseMessage(MessageType::APMFinalizingClose, sender)
    {}
} // namespace app::manager
