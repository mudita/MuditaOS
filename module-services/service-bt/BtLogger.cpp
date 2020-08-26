#include "BtLogger.hpp"
#include <vfs.hpp>
#include <log/log.hpp>
#include <cstring>
#include <time/time_conversion.hpp>
#include <mutex.hpp>

cpp_freertos::MutexStandard mutex;

BtLogger &BtLogger::get()
{
    static BtLogger *ptr = nullptr;
    if (ptr == nullptr) {
        ptr = new BtLogger("bt_log.txt");
    }
    return *ptr;
}

BtLogger::BtLogger(std::string name) : BtFile(name, "w")
{
    resp_buffer = new char[resp_buffer_size];
    last_flush = utils::time::Time().getTime();
}

void BtLogger::log(enum BtLogger::Event evt, const char* data, uint32_t size) 
{
    cpp_freertos::LockGuard lock(mutex);
    if (file != nullptr ) 
    {
        std::string header = c_str(evt);
        write(header.c_str(), header.length());
        write("[",  1);
        write(data, size);
        write("]\n",2);
    }
    if( evt == BtLogger::Event::In ) {
        flush();
    }
}

void BtLogger::log_out_byte(char byte)
{
    if( resp_buffer_take +1 != resp_buffer_size ) 
    {
        *(resp_buffer+resp_buffer_take) = byte;
        ++resp_buffer_take;
    }
    else
    {
        LOG_ERROR("bt log no more out space");
    }
}

void BtLogger::flush() 
{
    uint64_t now = utils::time::Time().getTime();
    last_flush = now;
    if ( file != nullptr && resp_buffer_take != 0 ) 
    {
        auto cmd_txt = [&]()->std::string{
                            std::stringstream ss;
                            for (int i = 0; i < resp_buffer_take; ++i) 
                            {
                                ss << ",0x" << std::setw(2) << std::setfill('0') << std::hex << (int)*(resp_buffer+i);
                            }
                            return ss.str();
                            }();

        log(BtLogger::Event::Out, cmd_txt.c_str(), cmd_txt.length());

        resp_buffer_take = 0;
    }
}

void BtLogger::timed_flush() 
{
    cpp_freertos::LockGuard lock(mutex);
    uint64_t now = utils::time::Time().getTime();
    if ( last_flush + 100 <  now ) {
        flush();
    }
}

BtLogger::~BtLogger()
{
    flush();
    delete[] resp_buffer;
}
