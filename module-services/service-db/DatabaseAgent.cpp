// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-db/DatabaseAgent.hpp"

namespace sys
{
    class Service;
} // namespace sys

DatabaseAgent::DatabaseAgent(sys::Service *parentService) : parentService(parentService)
{}
bool DatabaseAgent::storeIntoFile(const std::filesystem::path &file)
{
    if (database != nullptr) {
        return database->storeIntoFile(file);
    }

    return false;
}
