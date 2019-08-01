#pragma once
#include "../../BluetoothWorker.hpp"
#include "BtDev.hpp"
#include <bluetooth/Bluetooth.hpp>


class WorkerImpl : public BluetoothWorker
{
  public:
    static BluetoothWorker *create();
    WorkerImpl();
    ~WorkerImpl();
    virtual Error initialize_stack();

  private:
    std::unique_ptr<HCI> hci; /// our host controll interface - interface
    std::unique_ptr<BtDev> host; /// out 5b BT address
    std::vector<BtDev> connected;
    long unsigned int active_features;
    bsp::Bluetopia *bt;

    Error enable_a2dp();
    Error get_local_addr();
    Error enable_l2ca();
    Error hci_write_link_policy();
    Error check_a2dp();
    Error hcill_setup();
};
