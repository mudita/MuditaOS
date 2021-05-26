// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <Service/Message.hpp>

namespace message
{
    //! Inotify message received by the notifier class
    class inotify final : public ::sys::DataMessage
    {
      public:
    };
} // namespace message
