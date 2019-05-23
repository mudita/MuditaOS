/*
 *  @file db.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.05.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "db.hpp"
#include "log/log.hpp"


/* Declarations *********************/
extern sqlite3_vfs *sqlite3_ecophonevfs(void);

extern "C" {

int sqlite3_os_init(void) {
    /*
     ** The following macro defines an initializer for an sqlite3_vfs object.
     ** The name of the VFS is NAME.  The pAppData is a pointer to a pointer
     ** to the "finder" function.  (pAppData is a pointer to a pointer because
     ** silly C90 rules prohibit a void* from being cast to a function pointer
     ** and so we have to go through the intermediate pointer to avoid problems
     ** when compiling with -pedantic-errors on GCC.)
     **
     ** The FINDER parameter to this macro is the name of the pointer to the
     ** finder-function.  The finder-function returns a pointer to the
     ** sqlite_io_methods object that implements the desired locking
     ** behaviors.  See the division above that contains the IOMETHODS
     ** macro for addition information on finder-functions.
     **
     ** Most finders simply return a pointer to a fixed sqlite3_io_methods
     ** object.  But the "autolockIoFinder" available on MacOSX does a little
     ** more than that; it looks at the filesystem type that hosts the
     ** database file and tries to choose an locking method appropriate for
     ** that filesystem time.
     */

    sqlite3_vfs_register(sqlite3_ecophonevfs(), 1);

    return SQLITE_OK;
}

/*
 ** Shutdown the operating system interface.
 **
 ** Some operating systems might need to do some cleanup in this routine,
 ** to release dynamically allocated objects.  But not on unix.
 ** This routine is a no-op for unix.
 */
int sqlite3_os_end(void) {

    return SQLITE_OK;
}

/* Internal Defines ***********************/
void errorLogCallback(void *pArg, int iErrCode, const char *zMsg){
    LOG_ERROR("(%d) %s\n", iErrCode, zMsg);
}

}





void db::Init() {
    sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, (void*)1);  //(void*)1 is taken from official SQLITE examples and it appears that it ends variable args list
    sqlite3_initialize();
}