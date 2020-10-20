// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

namespace sys
{

    class Service;

    class Channel
    {

      public:
        Channel()
        {}

        std::vector<std::shared_ptr<Service>> m_services;
    };

} // namespace sys
