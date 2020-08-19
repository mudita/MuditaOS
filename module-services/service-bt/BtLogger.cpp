#include "BtLogger.hpp"
#include <vfs.hpp>
#include <log/log.hpp>
#include <cstring>
#include <time/time_conversion.hpp>

BtLogger::BtLogger(std::string name)
{
    LOG_DEBUG("opening log file: %s", vfs.relativeToRoot(name).c_str());
    file = vfs.fopen(name.c_str(), "w");
    if ( file == nullptr ) 
    {
        LOG_FATAL("bluetooth log file cant be created!");
    }

    resp_buffer = new char[resp_buffer_size];
    last_flush = utils::time::Time().getTime();
}

void BtLogger::write(const char *data, ssize_t size)
{
    if (file != nullptr) {
        vfs.fwrite(data, size,1, file);
    } else {
        LOG_ERROR("no file!");
    }
}

void BtLogger::log(enum BtLogger::Event evt, const char* data, uint32_t size) 
{
    if (file != nullptr ) 
    {
        LOG_DEBUG("logging bt: %s len: %" PRIu32, c_str(evt), size);
        std::string header = c_str(evt);
        write(header.c_str(), header.length());
        write("[", 1);
        for ( uint32_t i = 0; i < size; ++i ) {
            char hex[8] = {0};
            sprintf(hex, "0x%X ", *(data+i));
            write(hex, std::strlen(hex));
            LOG_DEBUG("%s", hex);
        }
        write("]\n",2);
    }
    flush();
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
    if ( file != nullptr && resp_buffer_take != 0 ) 
    {
        std::string header = c_str(BtLogger::Event::Out);
        write(header.c_str(), header.length());
        write("[", 1);
        for( int i =0; i < resp_buffer_take; ++i ) 
        {
            char hex[8] = {0};
            sprintf(hex, "0x%X ", *(resp_buffer+i));
            write(hex, std::strlen(hex));
        }
        write("]\n",2);
    }
}

void BtLogger::timed_flush() 
{
    uint64_t now = utils::time::Time().getTime();
    if ( last_flush + 100 <  now ) {
        flush();
        last_flush = now;
    }
}

BtLogger::~BtLogger()
{
    flush();
    vfs.fclose(file);
    delete[] resp_buffer;
}
