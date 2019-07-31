#include "bsp_bt.hpp"

namespace bsp {

BluetopiaHW::BluetopiaHW(LogLvl lvl) : Bluetopia(64,64,0)
{
    set_logging(lvl);
    log(LogDebug,"Init done!\n");
}

BluetopiaHW::~BluetopiaHW() {
}

Bluetopia *BluetopiaHW::getInstance()
{
    static BluetopiaHW *inst = NULL;
    if(inst==NULL) {
        inst = new BluetopiaHW();
    }
    return inst;
}

void Bluetopia::wait_data()
{
}

void Bluetopia::set_data()
{
}

void BluetopiaHW::open()
{
    log(LogDebug,"open!\n");
    set_irq(true);
}

void BluetopiaHW::close()
{
    log(LogDebug,"close!\n");
    set_irq(false);
}

void BluetopiaHW::sleep_ms(ssize_t ms)
{
    log(LogDebug,"sleep %d [ms]!\n", ms);
    ulTaskNotifyTake(pdTRUE, ms);
}

void mocup_write_blocking(unsigned char* buf, size_t len)
{
    printf("WRITE: [%d][%.*s]\n",int(len),int(len), buf);
}

BTdev::Error BluetopiaHW::flush()
{
    log(LogDebug,"flush [%d] %s\n", out.len, out.tail<out.head?"reverse":"normal");
    Error err = Success;
    int i=0;
    for(i=0; i<default_timeout_ms; ++i) {
        if(read_cts() == 0) {
            break;
        } else {
            sleep_ms(1);
        }
    }
    if(i==default_timeout_ms) {
        printf("BT CTS error!\n");
        err = ErrorTimeout;
    }
    char* from = &out.buff[out.head];
    if(out.tail >= out.head) {
        mocup_write_blocking(reinterpret_cast<uint8_t*>(from), out.len);
    } else {
        mocup_write_blocking(reinterpret_cast<uint8_t*>(from), out.len - out.tail);
        from = out.buff;
        mocup_write_blocking(reinterpret_cast<uint8_t*>(from), out.tail);
    }
    out.flush();
    return err;
}

ssize_t BluetopiaHW::write(char *buf, size_t nbytes)
{
    log(LogDebug, "write %d -> [%.*s]\n", nbytes, nbytes, buf);
    ssize_t i=0;
    // if CTS set -> ignore return 0, can use threshold_guard here too
    for (i=0; i<nbytes; ++i) {
        if(out.push(*(buf+i)) != 0) {
            break;
        }
    }
    return i;
}

ssize_t BluetopiaHW::write_blocking(char *buf, ssize_t len)
{
    int llen = 0;
    while (llen != len) {
        llen += write(buf, len-llen);
        flush();
    }
    return llen;
}

BTdev::Error BluetopiaHW::set_baudrate(uint32_t bd)
{
    log(LogDebug, "Set baudrate: %d", bd);
    Error ret = Success;
    int err=0;
    return ret;
}

// set flow on -> true, set flow off -> false
BTdev::Error BluetopiaHW::set_rts(bool on)
{
    log(LogDebug, "RTS %s", on?"on":"off");
    return Success;
}

BTdev::Error BluetopiaHW::set_reset(bool on)
{
    log(LogDebug, "reset %s", on?"on":"off");
    return Success;
}

void BluetopiaHW::set_irq(bool enable)
{
}

ssize_t BluetopiaHW::read(void*, unsigned long)
{
    return 0;
}

int BluetopiaHW::read_cts()
{
    return 0;
}

};
