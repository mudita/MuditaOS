// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Service/Message.hpp"

namespace sys
{
    enum class Code
    {
        CloseSystem,
        Reboot,
        None,
    };

    class SystemManagerCmd : public DataMessage
    {
      public:
        SystemManagerCmd(Code type) : DataMessage(BusChannels::SystemManagerRequests), type(type)
        {}

        Code type = Code::None;

        operator std::string() const override;
    };
} // namespace sys
