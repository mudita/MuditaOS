#pragma once

#include <mutex.hpp>

#include <string>

class Clipboard
{
    Clipboard() = default;

    std::string data;
    bool validData = false;

    static cpp_freertos::MutexStandard mutex;

  public:
    Clipboard(const Clipboard &) = delete;
    Clipboard &operator=(const Clipboard &) = delete;

    static Clipboard &getInstance();

    void copy(const std::string &);
    std::string paste();

    bool gotData() const
    {
        return validData;
    }
};
