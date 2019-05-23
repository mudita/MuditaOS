/*
 *  @file vfs.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.04.19
 *  @brief 
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "../../vfs.hpp"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstdio>
#include <cstddef>

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


namespace fs = std::filesystem;


vfs::vfs(){

}

vfs::~vfs(){

}

void vfs::Init(){
}

FILE * vfs::fopen ( const char * filename, const char * mode ){
    return std::fopen(filename,mode);
}

int vfs::fclose ( FILE * stream ){
    return std::fclose(stream);
}

int vfs::remove (const char *name){
    return std::remove(name);
}


size_t vfs::fread ( void * ptr, size_t size, size_t count, FILE * stream ){
    return std::fread(ptr,size,count,stream);
}

size_t vfs::fwrite ( const void * ptr, size_t size, size_t count, FILE * stream ){
    return std::fwrite(ptr,size,count,stream);
}

int vfs::fseek ( FILE * stream, long int offset, int origin ){
    return std::fseek(stream,offset,origin);
}

long int vfs::ftell ( FILE * stream ){
    return std::ftell(stream);
}

void vfs::rewind ( FILE * stream ){
    std::rewind(stream);
}

size_t vfs::filelength( FILE *stream ){

    size_t currPos = std::ftell(stream);

    std::fseek(stream,0,SEEK_END);
    size_t size = std::ftell(stream);
    std::fseek(stream,currPos,SEEK_SET);

    return size;
}

std::string vfs::getcurrdir(){
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        return std::string{cwd};
    } else {
        return "";
    }
}

std::vector<vfs::DirectoryEntry> vfs::listdir(const char* path)
{
    std::vector<DirectoryEntry> dir_list;
    FileAttributes  attribute;
    size_t fileSize = 0;


    for(auto& p: fs::directory_iterator(path))
    {
        if(fs::is_directory(p)){
            attribute = FileAttributes ::Directory;
        }
        else if((fs::status(p).permissions() & fs::perms::owner_write) != fs::perms::none){
            attribute = FileAttributes ::Writable;
            fileSize = std::filesystem::file_size(p);
        }
        else if((fs::status(p).permissions() & fs::perms::owner_read) != fs::perms::none){
            attribute = FileAttributes ::ReadOnly;
            fileSize = std::filesystem::file_size(p);
        }

        auto pathStr = p.path().string();

        auto path = pathStr.substr(pathStr.find_last_of("/\\")+1);

        dir_list.push_back(DirectoryEntry{path,attribute, static_cast<uint32_t >(fileSize)});
    }



    return dir_list;
}
