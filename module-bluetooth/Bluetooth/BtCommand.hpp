// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <FreeRTOS.h>
#include <task.h>

#include "BluetoothWorker.hpp"
#include "Error.hpp"
#include <functional>

namespace Bt
{
    auto initialize_stack() -> Error;
    auto register_hw_error_callback(std::function<void(uint8_t)> new_callback = nullptr) -> Error;
    auto set_name(std::string &name) -> Error;
    auto run_stack(TaskHandle_t *handle) -> Error;
    namespace GAP
    {
        extern std::vector<Devicei> devices;
        /// THIS have to be called prior to Bt system start!
        auto register_scan() -> Error;
        auto scan() -> Error;
        void stop_scan();
        auto set_visibility(bool visibility) -> Error;
        auto do_pairing(uint8_t *addr) -> bool;

        void setOwnerService(sys::Service *service);
    }; // namespace GAP
    namespace PAN
    {
        auto bnep_start() -> Error;
        auto bnep_setup() -> Error;
    } // namespace PAN
};    // namespace Bt
