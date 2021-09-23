// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace sys
{
    enum class ReturnCodes
    {
        Success,
        Failure,
        Timeout,
        ServiceDoesntExist,
        // This is used in application's template in base class messages handler. The meaning is that
        // message that was processed by base class implementation of the DataReceivedHandler was not processed
        // and it should be handled by the class next in hierarchy.
        Unresolved
    };
}
