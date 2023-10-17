#include "bsp_cellular.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/bsp/cellular/rt1051_cellular.hpp"
#elif defined(TARGET_Linux)
#include "cellular/linux_cellular.hpp"
#else
#error "Unsupported target"
#endif

namespace bsp
{
    std::optional<std::unique_ptr<Cellular>> Cellular::create([[maybe_unused]] const char *term, uint32_t portSpeed)
    {

        std::unique_ptr<Cellular> inst;

#if defined(TARGET_RT1051)
        inst = std::make_unique<bsp::RT1051Cellular>();
#elif defined(TARGET_Linux)
        inst = std::make_unique<bsp::LinuxCellular>(term, portSpeed);
#else
#error "Unsupported target"
#endif

        if (inst->isInitialized) {
            return inst;
        }

        return {};
    }

    [[nodiscard]] auto Cellular::getCellularDevice() const noexcept -> std::shared_ptr<devices::Device>
    {
        return driverLPUART;
    }

    [[nodiscard]] auto Cellular::getLastCommunicationTimestamp() const noexcept -> TickType_t
    {
        return lastCommunicationTimestamp;
    }

    [[nodiscard]] auto Cellular::isCellularInSleepMode() const noexcept -> bool
    {
        return isInSleepMode;
    }

} // namespace bsp
