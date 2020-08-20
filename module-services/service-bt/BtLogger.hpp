#pragma once

#include "BtFile.hpp"

/// support to write to some bt log...
class BtLogger : public BtFile
{
    std::string logname;
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
    ~BtLogger() override;
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
