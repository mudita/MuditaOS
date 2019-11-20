#pragma once
#include <FreeRTOS.h>
#include <task.h>

#include "BluetoothWorker.hpp"
#include "Error.hpp"
#include <functional>

namespace Bt {
Error initialize_stack();
Error register_hw_error_callback(std::function<void(uint8_t)> new_callback=nullptr);
Error set_name(std::string &name);
Error run_stack(TaskHandle_t *handle);
namespace GAP {
/// THIS have to be called prior to Bt system start!
Error register_scan();
Error scan();
};
namespace PAN {
Error bnep_start();
Error bnep_setup();
}
};
