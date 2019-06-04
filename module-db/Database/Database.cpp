
/*
 * @file Database.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "Database.hpp"
#include <assert.h>
#include <memory>
#include "log/log.hpp"
#include "vfs.hpp"

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


Database::Database(const char *name) :
        dbConnection(nullptr),
        dbName(name),
        isInitialized(false){

    auto rc = sqlite3_open(name, &dbConnection);
    assert(rc == SQLITE_OK);

}


Database::~Database() {
    sqlite3_close(dbConnection);
}

void Database::Initialize(){
    sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, (void*)1);  //(void*)1 is taken from official SQLITE examples and it appears that it ends variable args list
    sqlite3_initialize();
}
void Database::Deinitialize(){
    sqlite3_shutdown();
}

bool Database::Execute(const char *format, ...) {

    if (!format) { return false; }

    va_list ap;
    auto szQuery = std::make_unique<char[]>(maxQueryLen);
    va_start(ap, format);
    int res = vsnprintf(szQuery.get(), maxQueryLen, format, ap);
    va_end(ap);

    if (res == -1) {
        LOG_ERROR("SQL Query truncated (and not execute) for format: %s", format);
        return false;
    }


    int result = sqlite3_exec(dbConnection, szQuery.get(), NULL, NULL, NULL);

    return result != SQLITE_OK ? false : true;
}

std::unique_ptr<QueryResult> Database::Query(const char *format, ...) {

    if (!format) { return nullptr; }

    va_list ap;
    auto szQuery = std::make_unique<char[]>(maxQueryLen);
    va_start(ap, format);
    int res = vsnprintf(szQuery.get(), maxQueryLen, format, ap);
    va_end(ap);

    if (res == -1) {
        LOG_ERROR("SQL Query truncated (and not execute) for format: %s", format);
        return nullptr;
    }


    auto queryResult = std::make_unique<QueryResult>();

    int result = sqlite3_exec(dbConnection, szQuery.get(), queryCallback, queryResult.get(), NULL);
    if (result != SQLITE_OK) {
        LOG_ERROR("SQL query failed selecting : %d", result);
        return nullptr;
    }

    return queryResult;
}

int Database::queryCallback(void *usrPtr, int count, char **data, char **columns) {
    QueryResult *db = reinterpret_cast<QueryResult *>(usrPtr);

    std::vector<Field> row;
    for (uint32_t i = 0; i < count; i++) {
        row.push_back(Field{data[i]});
    }

    db->AddRow(row);

    return 0;
}

uint32_t Database::GetLastInsertRowID() {
    return sqlite3_last_insert_rowid(dbConnection);
}