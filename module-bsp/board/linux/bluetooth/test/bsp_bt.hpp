#pragma once

#include "bsp/bluetooth/Bluetooth.hpp"
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "timers.h"

#include <memory>

namespace bsp
{
    class BluetopiaHW : public Bluetopia
    {
      public:
        BluetopiaHW(LogLvl lvl = LogError);
        virtual ~BluetopiaHW();
        static Bluetopia *getInstance();
        static const ssize_t baudrate      = 115200;
        static const ssize_t off_threshold = 16;
        static const ssize_t on_threshold  = 32;

        // without them it doesnt see inherited functs o_o
        virtual void open() override;
        virtual void close() override;
        virtual void sleep_ms(ssize_t ms);
        virtual ssize_t read(void *buf, size_t nbytes) override;
        virtual ssize_t write(char *buf, size_t nbytes);
        virtual Error flush();
        virtual Error set_baudrate(uint32_t bd);
        virtual Error set_rts(bool on);
        virtual Error set_reset(bool on);
        virtual int read_cts();
        ssize_t write_blocking(char *buf, ssize_t len);
        virtual void set_irq(bool enable);
        virtual void wait_data();
        virtual void set_data();
    };
}; // namespace bsp
