/*
 *  @file vfs.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.04.19
 *  @brief 
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "../../vfs.hpp"
#include "eMMC/eMMC.hpp"
#include "ff_eMMC_user_disk.hpp"

vfs::vfs():
    emmc()
{

}

vfs::~vfs(){
    FF_eMMC_user_DiskDelete(emmcFFDisk);
    emmc.DeInit();
}

void vfs::Init(){
    emmc.Init();

    emmcFFDisk= FF_eMMC_user_DiskInit(eMMC_USER_DISK_NAME,&emmc);

    /* Print out information on the disk. */
    FF_eMMC_user_DiskShowPartition( emmcFFDisk );
}

vfs::FILE * vfs::fopen ( const char * filename, const char * mode ){
    return ff_fopen(filename,mode);
}

int vfs::fclose ( FILE * stream ){
    return ff_fclose(stream);
}

int vfs::remove (const char *name){
    return ff_remove(name);
}

size_t vfs::fread ( void * ptr, size_t size, size_t count, FILE * stream ){
    return ff_fread(ptr,size,count,stream);
}

size_t vfs::fwrite ( const void * ptr, size_t size, size_t count, FILE * stream ){
    return ff_fwrite(ptr,size,count,stream);
}

int vfs::fseek ( FILE * stream, long int offset, int origin ){
    return ff_fseek(stream,offset,origin);
}

long int vfs::ftell ( FILE * stream ){
    return ff_ftell(stream);
}

void vfs::rewind ( FILE * stream ){
    ff_rewind(stream);
}

size_t vfs::filelength( FILE *stream ){
    return ff_filelength(stream);
}

std::string vfs::getcurrdir(){
    char buff[64] = {};
    ff_getcwd(buff,sizeof buff);
    return std::string{buff};

}

std::vector<vfs::DirectoryEntry> vfs::listdir(const char* path){
    std::vector<DirectoryEntry> dir_list;

    FF_FindData_t *pxFindStruct;
    FileAttributes  attribute;

    /* FF_FindData_t can be large, so it is best to allocate the structure
    dynamically, rather than declare it as a stack variable. */
    pxFindStruct = ( FF_FindData_t * ) malloc( sizeof( FF_FindData_t ) );

    /* FF_FindData_t must be cleared to 0. */
    memset( pxFindStruct, 0x00, sizeof( FF_FindData_t ) );

    /* The first parameter to ff_findfist() is the directory being searched.  Do
    not add wildcards to the end of the directory name. */
    if( ff_findfirst( path, pxFindStruct ) == 0 )
    {
        do
        {
            /* Point pcAttrib to a string that describes the file. */
            if( ( pxFindStruct->ucAttributes & FF_FAT_ATTR_DIR ) != 0 )
            {
                attribute = FileAttributes::Directory;
            }
            else if( pxFindStruct->ucAttributes & FF_FAT_ATTR_READONLY )
            {
                attribute = FileAttributes::ReadOnly;
            }
            else
            {
                attribute = FileAttributes::Writable;
            }

            dir_list.push_back(DirectoryEntry{pxFindStruct->pcFileName,attribute,pxFindStruct->ulFileSize});

        } while( ff_findnext( pxFindStruct ) == 0 );
    }

    /* Free the allocated FF_FindData_t structure. */
    free( pxFindStruct );

    return dir_list;
}
