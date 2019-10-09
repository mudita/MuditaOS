#pragma once
#include <FreeRTOS.h>
#include <task.h>

#include "../../BluetoothWorker.hpp"


BluetoothWorker::Error initialize_stack(xQueueHandle qHandle);
