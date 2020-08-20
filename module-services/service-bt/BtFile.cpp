#include "BtFile.hpp"
#include <log/log.hpp>

BtFile::BtFile(const std::string &name)
{
    LOG_DEBUG("opening log file: %s", vfs.relativeToRoot(name).c_str());
    file = vfs.fopen(name.c_str(), "w");
    if ( file == nullptr ) 
    {
        LOG_FATAL("bluetooth log file cant be created!");
    }
}

void BtFile::write(const char *data, ssize_t size)
{
    if (file != nullptr) {
        vfs.fwrite(data, size,1, file);
    } else {
        LOG_ERROR("no file!");
    }
}


std::string BtFile::readline()
{
    if( file != nullptr ) 
    {
        return vfs.getline(file, 5 * 1024); /// 5kb.. top this is bad, this should be good enough
    }
    return "";
}

BtFile::~BtFile()
{
    vfs.fclose(file);
}
