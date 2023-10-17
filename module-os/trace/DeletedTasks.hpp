// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>
#include <cstdint>

namespace sys
{

    class DeletedTasks
    {

      public:
        struct DeletedTaskDetails_t
        {
            const char *name;             // A pointer to the task's name
            std::uint32_t tickIncrements; // The total run time allocated to the task so far, as defined by the run time
                                            // stats clock.
        };

        void MigrateDeletedTasks(std::vector<DeletedTaskDetails_t> &task);
    };

} // namespace sys

