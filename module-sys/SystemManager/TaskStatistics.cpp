// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SystemManager/TaskStatistics.hpp>
#include <log/log.hpp>
#include <semphr.h>
#include <Utils.hpp>
#include <algorithm>
#include <string>

namespace constants
{
    inline constexpr std::uint32_t cpuUsageThreshold{10};
    inline const std::string ignoredTaskName{"IDLE"};
} // namespace constants

namespace sys
{

    TaskStatistics::TaskStatistics() : totalSystemTick(0)
    {}

    void TaskStatistics::Update()
    {
        std::uint32_t currentSystemTick{0};
        const auto currentNumberOfTasks = uxTaskGetNumberOfTasks();

        TaskVector_t baseTasks;
        DeletedTaskVector_t delTasks;
        std::vector<TaskStatus_t> aliveTasks(currentNumberOfTasks);

        deletedTasks.MigrateDeletedTasks(delTasks);
        uxTaskGetSystemState(aliveTasks.data(), currentNumberOfTasks, &currentSystemTick);

        MergeDeletedTasks(baseTasks, delTasks);
        MergeAliveTasks(baseTasks, aliveTasks);
        MergeOldTasks(baseTasks, tasks);
        UpdateCpuUsage(baseTasks, utils::computeIncrease(currentSystemTick, totalSystemTick));
        ClearTasks(baseTasks);

        totalSystemTick = currentSystemTick;
    }

    void TaskStatistics::LogCpuUsage() const
    {
        for (auto &task : tasks) {
            if (!constants::ignoredTaskName.compare(task.name))
                continue;
            if (task.cpuUsage > constants::cpuUsageThreshold) {
                LOG_INFO("Task %s had %" PRIu32 "%% CPU usage in the last period.", task.name, task.cpuUsage);
            }
        }
    }

    void TaskStatistics::UpdateCpuUsage(TaskVector_t &baseTasks, const std::uint32_t systemTickIncrease)
    {
        for (auto &task : baseTasks) {
            task.cpuUsage = ComputePercentageCpuUsage(task.tickIncrements, systemTickIncrease);
        }
    }

    void TaskStatistics::MergeDeletedTasks(TaskVector_t &baseTasks, const DeletedTaskVector_t &tasksToMerge)
    {
        for (auto &task : tasksToMerge) {
            baseTasks.push_back({.name           = task.name,
                                 .isAlive        = false,
                                 .totalTick      = 0,
                                 .tickIncrements = task.tickIncrements,
                                 .cpuUsage       = 0});
        }
    }
    void TaskStatistics::MergeAliveTasks(TaskVector_t &baseTasks, const std::vector<TaskStatus_t> &tasksToMerge)
    {
        for (auto &task : tasksToMerge) {
            auto taskIt = std::find_if(
                std::begin(baseTasks), std::end(baseTasks), [task](auto &el) { return el.name == task.pcTaskName; });

            if (taskIt != std::end(baseTasks)) {
                taskIt->tickIncrements += task.ulRunTimeCounter;
                taskIt->totalTick = task.ulRunTimeCounter;
                taskIt->isAlive   = true;
            }
            else {
                baseTasks.push_back({.name           = task.pcTaskName,
                                     .isAlive        = true,
                                     .totalTick      = task.ulRunTimeCounter,
                                     .tickIncrements = task.ulRunTimeCounter,
                                     .cpuUsage       = 0});
            }
        }
    }
    void TaskStatistics::MergeOldTasks(TaskVector_t &baseTasks, const TaskVector_t &tasksToMerge)
    {
        for (auto &task : tasksToMerge) {
            if (!task.isAlive)
                continue;
            auto taskIt = std::find_if(
                std::begin(baseTasks), std::end(baseTasks), [task](auto &el) { return el.name == task.name; });

            if (taskIt != std::end(baseTasks)) {
                taskIt->tickIncrements -= task.totalTick;
            }
        }
    }
    void TaskStatistics::ClearTasks(TaskVector_t &baseTasks)
    {
        tasks.assign(baseTasks.begin(), baseTasks.end());
        baseTasks.clear();
    }

    [[nodiscard]] std::uint32_t TaskStatistics::ComputePercentageCpuUsage(const std::uint32_t taskTickIncrease,
                                                                          const std::uint32_t totalTickIncrease) const
    {
        return totalTickIncrease == 0u ? 0 : ((static_cast<std::uint64_t>(taskTickIncrease) * 100) / totalTickIncrease);
    }

} // namespace sys
