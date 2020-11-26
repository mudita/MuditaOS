#pragma once

#include <cstdint>      // uint32
#include <sys/types.h>  // ssize_t
#include <cstdarg>
#include <FreeRTOS.h>
#include <thread.hpp>
#include <board.h>

// edma
#include <module-bsp/board/rt1051/common/fsl_drivers/fsl_lpuart_edma.h>
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"
// /edma

/// c++ low level driver overlay

namespace bsp {

    class BTdev {
        public:
            enum Error {
                Success,
                ErrorUndefined,
                ErrorTimeout,
                ErrorBSP,
            };
            enum LogLvl {
                LogNone,
                LogError,
                LogWarning,
                LogDebug,
            };
            typedef int(*LogFoo)(const char*,va_list args);
        private:
            LogFoo flog;
        public:
            LogLvl loglvl;
            bool is_open;
            static const unsigned int default_timeout_ms = 1000;
            static const unsigned int default_buff_size = 1024;
            static const unsigned int default_baudrate = 115200;
            struct _circ {
                SemaphoreHandle_t sem = 0;
                void sem_take();
                void sem_give();
                char* buff;
                volatile unsigned int head, tail, threshold;
                const unsigned int size;
                volatile unsigned int len;
                _circ(unsigned int size, int threshold=0);
                ~_circ();
                inline int push(char val) {
                    LOG_DEBUG("circ push: %d", val);
                    sem_take();
                    int ret=0;
                    if(len<size) {
                        buff[tail]=val;
                        ++tail;
                        ++len;
                        if(tail==size) tail=0;
                    } else {
                        ret=-1;
                    }
                    sem_give();
                    return ret;
                }
                inline int pop(char* val) {
                    sem_take();
                    int ret=0;
                    if(val!=nullptr) {
                        if(len) {
                            *val = buff[head];
                            --len;
                            ++head;
                            if(head == size) head=0;
                        } else {
                            ret =-1;
                        }
                    } else {
                        ret=-2;
                    }
                    sem_give();
                    return ret;
                }
                // reached => 1, safe => 0
                inline bool threshold_guard() {
                    return len+threshold > size;
                }
                inline void flush() {
                    sem_take();
                    len=0;
                    head=0;
                    tail=0;
                    sem_give();
                }
            } in, out;

            BTdev(unsigned int in_size=default_buff_size, unsigned int out_size=default_buff_size, int threshold=0);
            virtual ~BTdev();

            // generall
            virtual void open() = 0;    // enable device -> irq enable
            virtual void close() = 0;   // disable device -> irq disable
            virtual ssize_t read(uint8_t *buf, size_t nbytes) = 0; // read from internal in buffer
            void log(LogLvl lvl,const char* val, ...);
            // uart specyfic
    };

    // Common stuff for Bluetopia and profiles +clean listing for overrrides
    class BluetoothCommon : public BTdev
    {
        public:
            static const ssize_t baudrate =115200;
            static const ssize_t off_threshold =16;
            static const ssize_t on_threshold =32;
        public:
            BluetoothCommon(unsigned int in_size=default_buff_size, unsigned int out_size=default_buff_size, int threshold=0);
            virtual ~BluetoothCommon();
            // uart specyfic Common part
            virtual void open() override;
            virtual void close() override;
            virtual ssize_t read(uint8_t *buf, size_t nbytes) override;
            virtual ssize_t write(const uint8_t *buf, size_t nbytes);
            virtual ssize_t write_blocking(const uint8_t *buf, ssize_t len);
            Error set_baudrate(uint32_t bd);
            Error set_reset(bool on);
            void sleep_ms(ssize_t ms);
            void set_irq(bool enable);

          private:
            void configure_uart_io();
            void configure_lpuart();
            void configure_cts_irq();
            void configure_lpuart_edma();
      public:
            // edma
            std::shared_ptr<drivers::DriverDMAMux> dmamux;
            std::shared_ptr<drivers::DriverDMA> dma;
            std::unique_ptr<drivers::DriverDMAHandle> uartRxDmaHandle;
            std::unique_ptr<drivers::DriverDMAHandle> uartTxDmaHandle;
            static AT_NONCACHEABLE_SECTION_INIT(lpuart_edma_handle_t uartDmaHandle);
            static void uartDmaCallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);
            static uint8_t dmaRXbuf[384];
            static uint32_t dmaRXreadCount;
            // /edma
    };

    /// definitions needed by BT stack

    class BlueKitchen : public BluetoothCommon {
        public:
            BlueKitchen(unsigned int in_size=default_buff_size, unsigned int out_size=default_buff_size);
            virtual ~BlueKitchen();
            static BlueKitchen *getInstance();

            virtual ssize_t read(uint8_t *buf, size_t nbytes) override;
            virtual ssize_t write(const uint8_t *buf, size_t nbytes) override;
            volatile uint32_t read_len = 0;
            volatile uint32_t to_read = 0;
            volatile char* read_buff;

            void set_flowcontrol(int on);

            void (*read_ready_cb)(void);
            void (*write_done_cb)(void);
            /// to be able to trigger events on thread
            xQueueHandle qHandle = nullptr;
    };
};
