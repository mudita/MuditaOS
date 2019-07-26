#pragma once

#include <cstdint>      // uint32
#include <sys/types.h>  // ssize_t
#include <cstdarg>
#include <FreeRTOS.h>
#include <thread.hpp>

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
                char* buff;
                unsigned int head, tail, threshold;
                const unsigned int size;
                volatile unsigned int len;
                _circ(unsigned int size, int threshold=0) : head(0), tail(0), threshold(threshold), size(size), len(0) { buff = new char[size];};
                ~_circ() { delete[] buff; }
                inline int push(char val) {
                    int ret=0;
                    if(len<size) {
                        buff[tail++]=val;
                        if(tail==size) tail=0;
                        ++len;
                    } else {
                        ret=-1;
                    }
                    return ret;
                }
                inline int pop(char* val) {
                    int ret=0;
                    if(val!=nullptr) {
                        if(len) {
                            *val = buff[head];
                            if(head == 0) head=size; else head--;
                            --len;
                        } else {
                            ret =-1;
                        }
                    } else {
                        ret=-2;
                    }
                    return ret;
                }
                // reached => 1, safe => 0
                inline bool threshold_guard() {
                    return len+threshold > size;
                }
                inline void flush() {
                    len=0;
                    head=tail=0;
                }
            } in, out;

            BTdev(unsigned int in_size=default_buff_size, unsigned int out_size=default_buff_size, int threshold=0);
            virtual ~BTdev();

            // device generall
            virtual void open() = 0;    // enable device -> irq enable
            virtual void close() = 0;   // disable device -> irq disable
            virtual void sleep_ms(ssize_t ms) = 0; // task sleep
            virtual ssize_t read(void *buf, size_t nbytes) = 0; // read from internal in buffor
            virtual ssize_t write(char *buf, size_t nbytes) = 0; // write to internal out buffor
            virtual Error flush() = 0;  // flush interal out buffor
            virtual void set_logging(LogLvl lvl) { loglvl = lvl; }
            void register_log(LogFoo foo) { flog = foo; }
            void log(LogLvl lvl,const char* val, ...);
            // uart specyfic
            virtual Error set_baudrate(uint32_t bd)=0;
            virtual Error set_rts(bool on)=0; // == virtual Error uart_flow(bool on);
            virtual Error set_reset(bool on)=0;
            virtual int read_cts()=0;
    };

    /// definitions needed by BT stack
    class Bluetopia : public BTdev {
        public:
            Bluetopia(unsigned int in_size=default_buff_size, unsigned int out_size=default_buff_size, int threshold=0);
            virtual ~Bluetopia();
            static Bluetopia *getInstance();
            virtual void set_irq(bool enable) = 0;
            virtual ssize_t write_blocking(char* buf, ssize_t len) =0;
            virtual void wait_data() = 0;
            virtual void set_data() =0;
            xSemaphoreHandle sem_data;

            void (*com_cb)(unsigned int transport_id, unsigned int datalen, unsigned char *buff, unsigned long param);
            unsigned long com_cb_param;
            long rx_thread;
            TaskHandle_t thandle;
    };


    class BlueKitchen : public BTdev {
        public:
            // uart
            BlueKitchen(unsigned int in_size=default_buff_size, unsigned int out_size=default_buff_size);
            int send_async();
            int recieve_async();
            int notify_send();
            int notify_received();
    };
};
