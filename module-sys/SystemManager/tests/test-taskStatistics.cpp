// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "SystemManager/TaskStatistics.hpp"

namespace mockup
{
    class TaskStat : public sys::TaskStatistics
    {
      public:
        using TaskVector_t        = std::vector<TaskDetails_t>;
        using DeletedTaskVector_t = std::vector<sys::DeletedTasks::DeletedTaskDetails_t>;

        [[nodiscard]] auto GetData() const noexcept -> TaskVector_t
        {
            return sys::TaskStatistics::tasks;
        }
        [[nodiscard]] auto GetPercentageCpuUsage(const std::uint32_t taskTickIncrease,
                                                 const std::uint32_t totalTickIncrease) const noexcept -> std::uint32_t
        {
            return ComputePercentageCpuUsage(taskTickIncrease, totalTickIncrease);
        }
        void mergeDeletedTasks(TaskVector_t &baseTasks, const DeletedTaskVector_t &tasksToMerge)
        {
            MergeDeletedTasks(baseTasks, tasksToMerge);
        }
        void mergeAliveTasks(TaskVector_t &baseTasks, const std::vector<TaskStatus_t> &tasksToMerge)
        {
            MergeAliveTasks(baseTasks, tasksToMerge);
        }
        void mergeOldTasks(TaskVector_t &baseTasks, const TaskVector_t &tasksToMerge)
        {
            MergeOldTasks(baseTasks, tasksToMerge);
        }
        void clearTasks(TaskVector_t &baseTasks)
        {
            ClearTasks(baseTasks);
        }
    };
} // namespace mockup

TEST_CASE("CpuUsage")
{
    auto taskStat = std::make_unique<mockup::TaskStat>();
    SECTION("positive")
    {
        const auto data = taskStat->GetPercentageCpuUsage(100, 1000);
        REQUIRE(data == 10);
    }
    SECTION("null")
    {
        const auto data = taskStat->GetPercentageCpuUsage(100, 0);
        REQUIRE(data == 0);
    }
    SECTION("overflow")
    {
        const auto data = taskStat->GetPercentageCpuUsage(50000000, 100000000);
        REQUIRE(data == 50);
    }
}

TEST_CASE("Tick Increments")
{
    auto taskStat = std::make_unique<mockup::TaskStat>();
    mockup::TaskStat::TaskVector_t baseTasks;
    mockup::TaskStat::DeletedTaskVector_t deletedTasks;
    std::vector<TaskStatus_t> aliveTasks;
    mockup::TaskStat::TaskVector_t oldTasks;

    oldTasks.push_back({.name = "test_1", .isAlive = true, .totalTick = 1000, .tickIncrements = 100, .cpuUsage = 0});
    oldTasks.push_back({.name = "test_2", .isAlive = true, .totalTick = 1100, .tickIncrements = 200, .cpuUsage = 0});
    oldTasks.push_back({.name = "test_3", .isAlive = false, .totalTick = 1200, .tickIncrements = 300, .cpuUsage = 0});
    oldTasks.push_back({.name = "test_4", .isAlive = false, .totalTick = 1300, .tickIncrements = 400, .cpuUsage = 0});
    oldTasks.push_back({.name = "test_5", .isAlive = true, .totalTick = 1400, .tickIncrements = 500, .cpuUsage = 0});
    oldTasks.push_back({.name = "test_6", .isAlive = false, .totalTick = 1500, .tickIncrements = 600, .cpuUsage = 0});

    aliveTasks.push_back({.xHandle              = NULL,
                          .pcTaskName           = "test_2",
                          .xTaskNumber          = 0,
                          .eCurrentState        = eTaskState::eSuspended,
                          .uxCurrentPriority    = 0,
                          .uxBasePriority       = 0,
                          .ulRunTimeCounter     = 1000,
                          .pxStackBase          = nullptr,
                          .usStackHighWaterMark = 0});
    aliveTasks.push_back({.xHandle              = NULL,
                          .pcTaskName           = "test_4",
                          .xTaskNumber          = 0,
                          .eCurrentState        = eTaskState::eSuspended,
                          .uxCurrentPriority    = 0,
                          .uxBasePriority       = 0,
                          .ulRunTimeCounter     = 1500,
                          .pxStackBase          = nullptr,
                          .usStackHighWaterMark = 0});
    aliveTasks.push_back({.xHandle              = NULL,
                          .pcTaskName           = "test_5",
                          .xTaskNumber          = 0,
                          .eCurrentState        = eTaskState::eSuspended,
                          .uxCurrentPriority    = 0,
                          .uxBasePriority       = 0,
                          .ulRunTimeCounter     = 2000,
                          .pxStackBase          = nullptr,
                          .usStackHighWaterMark = 0});
    aliveTasks.push_back({.xHandle              = NULL,
                          .pcTaskName           = "test_6",
                          .xTaskNumber          = 0,
                          .eCurrentState        = eTaskState::eSuspended,
                          .uxCurrentPriority    = 0,
                          .uxBasePriority       = 0,
                          .ulRunTimeCounter     = 2500,
                          .pxStackBase          = nullptr,
                          .usStackHighWaterMark = 0});

    deletedTasks.push_back({.name = "test_1", .tickIncrements = 1200});
    deletedTasks.push_back({.name = "test_2", .tickIncrements = 400});
    deletedTasks.push_back({.name = "test_3", .tickIncrements = 600});
    deletedTasks.push_back({.name = "test_4", .tickIncrements = 800});

    taskStat->mergeDeletedTasks(baseTasks, deletedTasks);
    taskStat->mergeAliveTasks(baseTasks, aliveTasks);
    taskStat->mergeOldTasks(baseTasks, oldTasks);
    taskStat->clearTasks(baseTasks);
    auto data = taskStat->GetData();

    REQUIRE(data[0].tickIncrements == 200);
    REQUIRE(data[1].tickIncrements == 300);
    REQUIRE(data[2].tickIncrements == 600);
    REQUIRE(data[3].tickIncrements == 2300);
    REQUIRE(data[4].tickIncrements == 600);
    REQUIRE(data[5].tickIncrements == 2500);
}
