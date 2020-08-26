#pragma once

#include <string>
#include <vfs.hpp> // this sucks - but need to be done for FILE

class BtFile
{
    protected:
    vfs::FILE* file = nullptr;
    std::string name,rwx;
    void open();
    void close();
    public:
    BtFile(const std::string &name, const std::string &rwx);
    virtual ~BtFile();
    void write(const char *data, ssize_t size);
    std::string readline();
};
