#pragma once

#include <string>
#include <vfs.hpp> // this sucks - but need to be done for FILE

/// support to write to some bt log...
class BtLogger
{
    std::string logname;
    vfs::FILE* file = nullptr;
    char* resp_buffer = nullptr;
    ssize_t resp_buffer_size = 512;
    ssize_t resp_buffer_take = 0;
    uint64_t last_flush = 0;
    public:
    enum class Event 
    {
        In,  /// incoming data on usb
        Out, /// outgoing data on usb
        USB_Error, /// usb error
    };
    BtLogger( std::string name);
    ~BtLogger();
    void write(const char *data, ssize_t size);
    void log(enum BtLogger::Event evt, const char* data, uint32_t size);
    void log_out_byte(char byte);
    void flush();
    /// if there was no communication for some time we might want to check
    /// if there was no data to flush
    /// otherwise we would just get data after next command
    void timed_flush();
};

inline const char* c_str(BtLogger::Event evt) 
{
    switch ( evt ) 
    {
        case BtLogger::Event::In:
            return "< ";
        case BtLogger::Event::Out:
            return "> ";
        case BtLogger::Event::USB_Error:
            return "usb_error: ";
        default:
            return "?: ";
    }
    return "?: ";
}
