#pragma once

#include <cstdint>     // uint32
#include <sys/types.h> // ssize_t
#include <cstdarg>
#include <FreeRTOS.h>
#include <thread.hpp>
#include <board.h>

#if defined(TARGET_RT1051)
#include "board/rt1051/common/fsl_drivers/fsl_lpuart_edma.h"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"
#endif

/// c++ low level driver overlay

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

    class BTdev
    {
      public:
        enum Error
        {
            Success,
            ErrorUndefined,
            ErrorTimeout,
            ErrorBSP,
        };
        enum LogLvl
        {
            LogNone,
            LogError,
            LogWarning,
            LogDebug,
        };
        typedef int (*LogFoo)(const char *, va_list args);

      private:
        LogFoo flog;

      public:
        LogLvl loglvl;
        bool is_open;
        static const unsigned int default_timeout_ms = 1000;
        static const unsigned int default_buff_size  = 1024;
        static const unsigned int default_baudrate   = 115200;

        BTdev();
        virtual ~BTdev();

        // general
        virtual void open()                                    = 0; // enable device -> irq enable
        virtual void close()                                   = 0; // disable device -> irq disable
        virtual BTdev::Error read(uint8_t *buf, size_t nbytes) = 0;
        void log(LogLvl lvl, const char *val, ...);
        // uart specific
    };

    class BluetoothCommon : public BTdev
    {
      public:
        static const ssize_t baudrate      = 115200;
        static const ssize_t off_threshold = 16;
        static const ssize_t on_threshold  = 32;

        BluetoothCommon();
        virtual ~BluetoothCommon();
        // uart specific Common part
        virtual void open() override;
        virtual void close() override;
        virtual BTdev::Error read(uint8_t *buf, size_t nbytes) override;
        virtual BTdev::Error write(const uint8_t *buf, size_t nbytes);
        virtual ssize_t write_blocking(const uint8_t *buf, ssize_t nbytes);
        Error set_baudrate(uint32_t bd);
        Error set_reset(bool on);
        void sleep_ms(ssize_t ms);
        void set_irq(bool enable);

      private:
        void init_uart();
        void deinit_uart();
        void init_uart_dma();
        void deinit_uart_dma();

#if defined(TARGET_RT1051)
        std::shared_ptr<drivers::DriverDMAMux> dmamux;
        std::shared_ptr<drivers::DriverDMA> dma;
        std::unique_ptr<drivers::DriverDMAHandle> uartRxDmaHandle;
        std::unique_ptr<drivers::DriverDMAHandle> uartTxDmaHandle;
        static AT_NONCACHEABLE_SECTION_INIT(lpuart_edma_handle_t uartDmaHandle);
        static void uartDmaCallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);
#endif
    };

    /// definitions needed by BT stack

    class BlueKitchen : public BluetoothCommon
    {
      public:
        BlueKitchen();
        virtual ~BlueKitchen();
        static BlueKitchen *getInstance();

        virtual BTdev::Error read(uint8_t *buf, size_t nbytes) override;
        virtual BTdev::Error write(const uint8_t *buf, size_t size) override;
        uint32_t read_len = 0;
        uint8_t *read_buff;

        void set_flowcontrol(int on);

        void (*read_ready_cb)(void);
        void (*write_done_cb)(void);
        /// to be able to trigger events on thread
        xQueueHandle qHandle = nullptr;
    };
}; // namespace bsp
