
/*
 * @file Database.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "Database.hpp"
#include "log/log.hpp"
#include "vfs.hpp"
#include <assert.h>
#include <memory>

/* Declarations *********************/
extern sqlite3_vfs *sqlite3_ecophonevfs(void);

extern "C"
{

    int sqlite3_os_init(void)
    {
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
    int sqlite3_os_end(void)
    {

        return SQLITE_OK;
    }

    /* Internal Defines ***********************/
    void errorLogCallback(void *pArg, int iErrCode, const char *zMsg)
    {
        LOG_ERROR("(%d) %s\n", iErrCode, zMsg);
    }
}

Database::Database(const char *name) : dbConnection(nullptr), dbName(name), isInitialized_(false)
{
    LOG_INFO("creating database: %s", dbName);
    auto rc = sqlite3_open(name, &dbConnection);
    if (rc != SQLITE_OK) {
        LOG_ERROR("SQLITE INITIALIZATION ERROR! rc=%d dbName=%s", rc, name);
    }
    assert(rc == SQLITE_OK);
    pragmaQuery("PRAGMA integrity_check;");
    pragmaQuery("PRAGMA locking_mode=EXCLUSIVE");
    pragmaQuery("PRAGMA journal_mode=WAL");
}

Database::~Database()
{
    sqlite3_close(dbConnection);
}

void Database::initialize()
{
    sqlite3_config(
        SQLITE_CONFIG_LOG,
        errorLogCallback,
        (void *)1); //(void*)1 is taken from official SQLITE examples and it appears that it ends variable args list
    sqlite3_initialize();
}
void Database::deinitialize()
{
    sqlite3_shutdown();
}

bool Database::execute(const char *format, ...)
{
    if (!format) {
        return false;
    }

    va_list ap;
    char *szQuery = static_cast<char *>(sqlite3_malloc(maxQueryLen));
    va_start(ap, format);
    sqlite3_vsnprintf(maxQueryLen, (char *)szQuery, format, ap);
    va_end(ap);

    int result = sqlite3_exec(dbConnection, szQuery, NULL, NULL, NULL);
    if (result != SQLITE_OK)
        LOG_ERROR("Execution of \'%s\' failed with %d", szQuery, result);

    sqlite3_free(szQuery);

    return result != SQLITE_OK ? false : true;
}

std::unique_ptr<QueryResult> Database::query(const char *format, ...)
{

    if (!format) {
        return nullptr;
    }

    va_list ap;
    char *szQuery = static_cast<char *>(sqlite3_malloc(maxQueryLen));
    va_start(ap, format);
    szQuery[0] = 0;
    sqlite3_vsnprintf(maxQueryLen, szQuery, format, ap);
    va_end(ap);

    auto queryResult = std::make_unique<QueryResult>();

    int result = sqlite3_exec(dbConnection, szQuery, queryCallback, queryResult.get(), NULL);
    if (result != SQLITE_OK) {
        LOG_ERROR("SQL query \'%s\' failed selecting : %d", szQuery, result);
        return nullptr;
    }

    sqlite3_free(szQuery);

    return queryResult;
}

int Database::queryCallback(void *usrPtr, int count, char **data, char **columns)
{
    QueryResult *db = reinterpret_cast<QueryResult *>(usrPtr);

    std::vector<Field> row;
    for (uint32_t i = 0; i < (uint32_t)count; i++) {
        try {
            row.push_back(Field{data[i]});
        }
        catch (...) {
            LOG_FATAL("Error on: %" PRIu32 " %s", i, data[i]);
        }
    }

    db->addRow(row);

    return 0;
}

uint32_t Database::getLastInsertRowId()
{
    return sqlite3_last_insert_rowid(dbConnection);
}

void Database::pragmaQuery(const std::string &pragmaStatemnt)
{
    auto results = query(pragmaStatemnt.c_str());
    if (results) {
        uint32_t fieldsCount = results->getFieldCount();
        LOG_DEBUG("Row count: %" PRIu32 " Fields count:%" PRIu32, results->getRowCount(), fieldsCount);
        do {
            for (uint32_t i = 0; i < fieldsCount; i++) {
                Field field = (*results)[i];
                LOG_DEBUG("%s: '%s'", pragmaStatemnt.c_str(), field.getCString());
            }
        } while (results->nextRow());
    }
    else {
        LOG_DEBUG("no results!");
    }
}

bool Database::storeIntoFile(const std::string &backupPath)
{
    LOG_INFO("Backup database: %s, into file: %s - STARTED", dbName, backupPath.c_str());

    auto rc = execute("VACUUM INTO '%q';", backupPath.c_str());

    if (rc == true) {
        LOG_INFO("Backup database: %s, into file: %s - SUCCEDED", dbName, backupPath.c_str());
    }
    else {
        LOG_ERROR("Backup database: %s, into file: %s - FAILED", dbName, backupPath.c_str());
    }

    return rc;
}
