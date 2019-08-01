#pragma once
#include <FreeRTOS.h>
#include <task.h>

#include "../../BluetoothWorker.hpp"

class WorkerImpl : public BluetoothWorker
{
  public:
    WorkerImpl();
    ~WorkerImpl();
    static WorkerImpl *create();
    virtual Error initialize_stack();
    Error aud_init();
    xTaskHandle handle;
};
