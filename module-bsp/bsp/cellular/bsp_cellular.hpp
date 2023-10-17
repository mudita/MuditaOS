#ifndef PUREPHONE_BSP_CELLULAR_HPP
#define PUREPHONE_BSP_CELLULAR_HPP

#include <bsp/common.hpp>

#include <optional>
#include <stdint.h>
#include <stddef.h>
#include <memory>
#include <chrono>
#include <FreeRTOS.h>
#include <queue.h>
#include "drivers/lpuart/DriverLPUART.hpp"
#include <EventStore.hpp>

namespace bsp
{
    namespace cellular
    {
        enum class antenna
        {
            lowBand,
            highBand
        };

    }
    class Cellular
    {
      public:
        static std::optional<std::unique_ptr<Cellular>> create(const char *term   = "/dev/ttyUSB0",
                                                               uint32_t portSpeed = 115200);

        Cellular()          = default;
        virtual ~Cellular() = default;

        virtual void powerUp() = 0;

        virtual void powerDown() = 0;

        virtual void restart() = 0;

        virtual uint32_t wait(std::chrono::milliseconds timeoutMs) = 0;

        virtual ssize_t read(void *buf, size_t nbytes, std::chrono::milliseconds timeoutMs) = 0;

        virtual ssize_t write(void *buf, size_t nbytes) = 0;

        virtual void informModemHostAsleep() = 0;

        virtual void informModemHostWakeup() = 0;

        virtual void enterSleep() = 0;

        virtual void exitSleep() = 0;

        virtual void setSpeed(uint32_t portSpeed) = 0;

        virtual void setSendingAllowed(bool state) = 0;

        virtual bool getSendingAllowed() const noexcept = 0;

        virtual void selectAntenna(bsp::cellular::antenna antenna) = 0;
        virtual bsp::cellular::antenna getAntenna()                = 0;

        virtual bsp::Board getBoard() = 0;

        [[nodiscard]] auto getCellularDevice() const noexcept -> std::shared_ptr<devices::Device>;
        [[nodiscard]] auto getLastCommunicationTimestamp() const noexcept -> TickType_t;
        [[nodiscard]] auto isCellularInSleepMode() const noexcept -> bool;

      protected:
        bool isInitialized = false;
        bool isInSleepMode{false};
        TickType_t lastCommunicationTimestamp;

        std::shared_ptr<drivers::DriverLPUART> driverLPUART;
    };
    namespace cellular
    {
        enum IRQsource
        {
            statusPin,
            trayPin,
            ringIndicatorPin,
        };

        /// initialize SIM queue directed to EventWorker
        int init(xQueueHandle qHandle);

        namespace USB
        {
            enum class PassthroughState
            {
                ENABLED,
                DISABLED,
            };

            enum class BootPinState
            {
                FIRMWARE_UPGRADE,
                NORMAL_BOOT,
            };

            BootPinState getBootPin();
            PassthroughState getPassthrough();

            void setBootPin(BootPinState bootPin);
            void setPassthrough(PassthroughState pass);
        } // namespace USB

        namespace status
        {
            enum class value
            {
                /// from the docs: When the module is turned on normally, the STATUS will present the low state.
                ACTIVE,
                INACTIVE,
            };
            /// returns true if status OK. Open drain pulled up; ground is OK.
            bsp::cellular::status::value getStatus();
            BaseType_t statusIRQhandler();
        } // namespace status

        namespace sim
        {
            /// handler for SIM tray which is connected to phone, not GSM
            BaseType_t trayIRQHandler();

            Store::GSM::Tray getTray();
            /// trigger swap pin on gsm so that it would reload sim card in tray
            /// after that +QPIN urc should come
            void hotSwapTrigger();
            void simSelect();
        } // namespace sim

        namespace ringIndicator
        {
            // handling incoming calls and sms - RI pin
            BaseType_t riIRQHandler();
        } // namespace ringIndicator

    } // namespace cellular
};    // namespace bsp

#endif // PUREPHONE_BSP_CELLULAR_HPP
