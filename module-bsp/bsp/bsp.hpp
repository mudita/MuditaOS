#pragma once

#include <cstdint>

namespace bsp
{
    enum class RebootState : std::uintptr_t
    {
        None,
        Poweroff,
        Reboot
    };

    void board_init();
    void board_power_off();
    void board_restart();

    /// Register platform exit functions
    int register_exit_function(void (*func)());

    /// Board-specific configuration procedure. Currently used to perform WFI-related config for Harmony.
    void board_configure();

    /// Board-specific exit/reset procedure. It is the main exit point from the system.
    [[noreturn]] void board_exit(RebootState state);
} // namespace bsp
