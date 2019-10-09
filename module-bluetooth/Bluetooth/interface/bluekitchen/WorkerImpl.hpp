#pragma once
#include <FreeRTOS.h>
#include <task.h>

#include "../../BluetoothWorker.hpp"


BluetoothWorker::Error initialize_stack();
BluetoothWorker::Error run_stack(TaskHandle_t *handle);
BluetoothWorker::Error pan_bnep_setup();
