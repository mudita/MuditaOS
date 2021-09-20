/**
 * @file config.h
 * @author Lukasz Skrzypczak (lukasz.skrypczak@mudita.com)
 * @date 21 cze 2018
 * @brief Insert brief information about this file purpose.
 * @copyright Copyright (C) 2018 mudita.com.
 * @details More detailed information related to this code.
 */

#pragma once

#define SQLITE_OS_OTHER     1   //SQLITE has definitions for major OSes - UNIX, WIN etc. This define indicates that no known (at least to SQLITE) of is used
#define SQLITE_TEMP_STORE   3   //Temporary files. The user must configure SQLite to use in-memory temp files when using this VFS
#define SQLITE_THREADSAFE   0   //Use serialized thread-safe mode. This is fully supported threading environment setting
#define SQLITE_MEMDEBUG     0   //Not sure what exactly this do but without this SQLITE crashes
#define SQLITE_OMIT_AUTOINIT 1  // If this is set user has to manually invoke sqlite3_initialize.
#define SQLITE_DEFAULT_MEMSTATUS 0

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Woverflow"

