// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstddef>
struct task_prof_data;
namespace sys::cpu
{
    struct UpdateResult;

    namespace stats
    {

        class Printer
        {
          public:
            virtual void printSysUsage(struct task_prof_data *data, size_t size) = 0;
            virtual void printCPUChange(const cpu::UpdateResult &ret)            = 0;
            virtual void printPowerConsumption()     = 0;
        };

        class NullPrinter : public Printer
        {
            void printSysUsage(struct task_prof_data *data, size_t size)
            {}
            void printCPUChange(const cpu::UpdateResult &ret)
            {}
            void printPowerConsumption() {}
        };

        class LogPrinter : public Printer
        {
          public:
            void printSysUsage(struct task_prof_data *data, size_t size) override;
            void printCPUChange(const cpu::UpdateResult &ret) override;
            void printPowerConsumption() override;
        };

        class PackPrinter : public Printer
        {
          public:
            void printSysUsage(struct task_prof_data *data, size_t size) override;
            void printCPUChange(const cpu::UpdateResult &ret) override;
            void printPowerConsumption() override;
        };
    }; // namespace stats
};     // namespace sys::cpu
