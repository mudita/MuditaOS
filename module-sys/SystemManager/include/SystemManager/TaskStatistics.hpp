// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <vector>
#include <cstdint>
#include <DeletedTasks.hpp>

namespace sys
{

    class TaskStatistics
    {

      public:
        TaskStatistics();

        /// update the total running time allocated so far for all tasks
        void Update();
        /// print the percentage of CPU usage in the last period
        void LogCpuUsage() const;

      protected:
        struct TaskDetails_t
        {
            const char *name;             // A pointer to the task's name
            bool isAlive;                 // Flag with information if task exists
            std::uint32_t totalTick;      // The total run time allocated to the task so far
            std::uint32_t tickIncrements; // run time allocated to the task for the last period
            std::uint32_t cpuUsage;       // Percentage of CPU usage for the last period
        };
        using TaskVector_t        = std::vector<TaskDetails_t>;
        using DeletedTaskVector_t = std::vector<DeletedTasks::DeletedTaskDetails_t>;

        TaskVector_t tasks;
        std::uint32_t totalSystemTick;
        DeletedTasks deletedTasks;

        [[nodiscard]] std::uint32_t ComputePercentageCpuUsage(const std::uint32_t taskTickIncrease,
                                                              const std::uint32_t totalTickIncrease) const;
        void UpdateCpuUsage(TaskVector_t &baseTasks, const std::uint32_t systemTickIncrease);
        void MergeDeletedTasks(TaskVector_t &baseTasks, const DeletedTaskVector_t &tasksToMerge);
        void MergeAliveTasks(TaskVector_t &baseTasks, const std::vector<TaskStatus_t> &tasksToMerge);
        void MergeOldTasks(TaskVector_t &baseTasks, const TaskVector_t &tasksToMerge);
        void ClearTasks(TaskVector_t &baseTasks);
    };

} // namespace sys
