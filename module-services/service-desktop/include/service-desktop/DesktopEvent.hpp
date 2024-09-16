// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <endpoints/Context.hpp>

namespace sdesktop
{
    class Event
    {
      protected:
        endpoints::Context context;

      public:
        void send();
        virtual ~Event() = default;
    };
} // namespace sdesktop
