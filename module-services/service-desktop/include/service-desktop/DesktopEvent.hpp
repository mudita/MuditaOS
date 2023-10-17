// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
