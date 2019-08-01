#pragma once
#include "../../BluetoothWorker.hpp"

class WorkerImpl : public BluetoothWorker
{
  public:
    WorkerImpl();
    static WorkerImpl *create();
    virtual Error initialize_stack();
    Error aud_init();
};
