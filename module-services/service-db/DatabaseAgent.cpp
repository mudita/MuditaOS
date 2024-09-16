// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "service-db/DatabaseAgent.hpp"

namespace sys
{
    class Service;
} // namespace sys

DatabaseAgent::DatabaseAgent(sys::Service *parentService) : parentService(parentService)
{}
