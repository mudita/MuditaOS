#pragma once

#include <cstdint>
#include <sys/types.h>
#include <cstdarg>
#include <FreeRTOS.h>
#include <thread.hpp>
#include <board.h>

#if defined(TARGET_RT1051)
#include "board/rt1051/common/fsl_drivers/fsl_lpuart_edma.h"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"
#endif

/// C++ low level driver overlay

namespace bsp
{
    namespace bluetooth
    {
        enum Message : std::uint8_t
        {
            /// asynchronous messages to use on event from irq
            EvtSending,        /// Bt stack ordered a write transaction and it is pending
            EvtSent,           /// trigger Bt stack wrote, enable writting in HCI in BluetoothWorker task
            EvtSendingError,   /// bsp error on send
            EvtReceiving,      /// Bt stack requested a receive transaction and it is pending
            EvtReceived,       /// trigger Bt stack received, start processing HCI in BluetoothWorker task
            EvtRecUnwanted,    /// not requested recieve - probably receive came to fast from sent...
            EvtReceivingError, /// bsp error on receive
            EvtUartError,      /// generic uart error
            EvtErrorRec,       /// there was error o queue receive
        };
    }

    class BTDevice
    {
      public:
        enum Error
        {
            Success,
            ErrorUndefined,
            ErrorTimeout,
            ErrorBSP,
        };

        enum LogLevel
        {
            LogNone,
            LogError,
            LogWarning,
            LogDebug,
        };

        using LogFunction = int (*)(const char *msg, va_list args);

        BTDevice();
        virtual ~BTDevice() = default;

        virtual void open() = 0;    // enable device -> irq enable
        virtual void close() = 0;   // disable device -> irq disable

        virtual Error read(std::uint8_t *buf, std::size_t size) = 0;

        void log(LogLevel level, const char *msg, ...);

        LogLevel logLevel;
        bool isOpen;

      private:
        LogFunction logFunction;
    };

    class BluetoothCommon : public BTDevice
    {
      public:
        static constexpr std::uint32_t defaultBaudRate = 115200;

        BluetoothCommon();
        virtual ~BluetoothCommon();

        // uart specific Common part
        virtual void open() override;
        virtual void close() override;

        virtual Error read(std::uint8_t *buf, std::size_t size) override;
        virtual Error write(const std::uint8_t *buf, std::size_t size);

        Error setBaudrate(std::uint32_t baud);
        Error setReset(bool on);
        void setIrq(bool enable);

        void sleepMs(std::size_t ms);

      private:
        void uartInit();
        void uartDeinit();

        void uartDmaInit();
        void uartDmaDeinit();

#if defined(TARGET_RT1051)
        std::shared_ptr<drivers::DriverDMAMux> dmamux;
        std::shared_ptr<drivers::DriverDMA> dma;
        std::unique_ptr<drivers::DriverDMAHandle> uartRxDmaHandle;
        std::unique_ptr<drivers::DriverDMAHandle> uartTxDmaHandle;
        static AT_NONCACHEABLE_SECTION_INIT(lpuart_edma_handle_t uartDmaHandle);

        static void uartDmaCallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);
#endif
    };

    /// Definitions needed by BT stack
    class BlueKitchen : public BluetoothCommon
    {
      public:
        BlueKitchen();
        virtual ~BlueKitchen();
        static BlueKitchen &getInstance();

        virtual Error read(uint8_t *buf, size_t size) override;
        virtual Error write(const uint8_t *buf, size_t size) override;

        std::uint32_t readLength = 0;
        std::uint8_t *readBuffer;

        void (*readReadyCallback)();
        void (*writeDoneCallback)();

        /// to be able to trigger events on thread
        xQueueHandle qHandle = nullptr;
    };
}
