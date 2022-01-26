#pragma once

#include <cstdint>

namespace bsp
{
    enum class rebootState : uintptr_t
    {
        none,
        poweroff,
        reboot
    };

    void board_init();

    void board_power_off();

    void board_restart();

    /// Register platform exit functions
    int register_exit_functions(void (*func)());

    /// Board-specific exit/reset procedure. It is the main exit point from the system.
    [[noreturn]] void board_exit(rebootState);
} // namespace bsp
