// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeletedTasks.hpp"
#include <FreeRTOS.h>
#include <mutex.hpp>
#include <task.h>
#include <cstdint>
#include <algorithm>

namespace
{
    std::vector<sys::DeletedTasks::DeletedTaskDetails_t> deletedTasks;
    cpp_freertos::MutexStandard mutex;
} // namespace

// Function called from FreeRTOS when the task is deleted, works on static data therefore it is necessary to ensure
// mutex synchronization
void trace_deleteTask(const char *name)
{
    TaskHandle_t xHandle;
    TaskStatus_t xTaskDetails;

    cpp_freertos::LockGuard lock(mutex);

    xHandle = xTaskGetHandle(name);
    if (xHandle != NULL) {
        vTaskGetInfo(xHandle, &xTaskDetails, pdFALSE, eInvalid);

        auto taskIt = std::find_if(std::begin(deletedTasks), std::end(deletedTasks), [xTaskDetails](auto &el) {
            return el.name == xTaskDetails.pcTaskName;
        });

        if (taskIt != std::end(deletedTasks)) {
            taskIt->tickIncrements += xTaskDetails.ulRunTimeCounter;
        }
        else {
            deletedTasks.push_back(
                {.name = xTaskDetails.pcTaskName, .tickIncrements = xTaskDetails.ulRunTimeCounter});
        }
    }
}

namespace sys
{
    void DeletedTasks::MigrateDeletedTasks(std::vector<DeletedTaskDetails_t> &task)
    {
        cpp_freertos::LockGuard lock(mutex);

        task.assign(deletedTasks.begin(), deletedTasks.end()); 
        deletedTasks.clear();
    }
}
