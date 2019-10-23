#pragma once
#include <FreeRTOS.h>
#include <task.h>

#include "BluetoothWorker.hpp"
#include "Error.hpp"

namespace Bt {
Error initialize_stack();
Error set_name(std::string &name);
Error run_stack(TaskHandle_t *handle);
namespace GAP {
/// THIS have to be called prior to Bt system start!
Error register_scan();
Error scan();
};
namespace PAN {
Error bnep_setup();
}
};
