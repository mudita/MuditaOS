#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <bsp/bluetooth/Bluetooth.hpp>
#include <memory>
#include <vector>

#include "profiles/BtProfile.hpp"
#include "profiles/GAP.hpp"
#include "Device.hpp"

struct HCI;

class BluetoothWorker {
    public:
        enum Error {
            SuccessBt,
            ErrorBtGeneric,
            ErrorBtAPI,
        };
    BluetoothWorker();
    ~BluetoothWorker();
    static BluetoothWorker* create();
    virtual Error initialize_stack() =0;

    std::vector<Device> scan();
    Error aud_init();

    /// bluetooth stack id in use
    unsigned long active_features;
    // TODO store profiles, now dangling
    Stack stack;
};
