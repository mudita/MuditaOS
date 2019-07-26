#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <bsp/bluetooth/Bluetooth.hpp>
#include <memory>
#include <vector>
#include "profiles/BtProfile.hpp"
#include "Stack.hpp"

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
    private:
        bsp::Bluetopia *bt; // lazy man ptr -> instead getting it all the time
        TaskHandle_t *handle = nullptr;
        /// start bluetopia stack
        Error initialize_stack();
        /// enable a2dp proile
        Error enable_a2dp();
        /// format and setup our extended inquiry response data
        // Error format_eir_data();
        /// get our bt address
        Error get_local_addr();
        /// enable L2CA link  - logical link controll adaptation, needed i.e. to enable any profile :)
        Error enable_l2ca();
        /// bluetooth HCI command support set master slave and sniff
        Error hci_write_link_policy();
        /// querry a2dp successfully enabled
        Error check_a2dp();
        /// initialize audio video interface
        Error aud_init();
        /// enables low power uart mode
        Error hcill_setup();

        /// out 5b BT address
        std::unique_ptr<BtDev> host;
        std::vector<BtDev> connected;
        std::unique_ptr<HCI> hci; /// our host controll interface - interface
        /// bluetooth stack id in use
        Stack stack;
        unsigned long active_features;
        std::vector<BtProfile*> profiles;
};
