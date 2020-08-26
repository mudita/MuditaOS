#include "BtFile.hpp"
#include <log/log.hpp>

BtFile::BtFile(const std::string &name,  const std::string &rwx) : name (name), rwx(rwx)
{
    LOG_DEBUG("opening log file: %s", vfs.relativeToRoot(name).c_str());
    file = vfs.fopen(name.c_str(), rwx.c_str());
    if ( file == nullptr ) 
    {
        LOG_FATAL("bluetooth log file cant be created!");
    }
}

// for write only really...
void BtFile::open()
{
    file = vfs.fopen(name.c_str(), "w+");
}

void BtFile::close()
{
    vfs.fclose(file);
}

void BtFile::write(const char *data, ssize_t size)
{
    open();
    if (file != nullptr) {
        vfs.fwrite(data, 1,size, file);
    } else {
        LOG_ERROR("no file!");
    }
    close();
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
    close();
}
