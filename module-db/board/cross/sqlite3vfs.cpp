/*

 * @file sqlite3port.c
 * @author Lukasz Skrzypczak (lukasz.skrypczak@mudita.com)
 * @date 21 cze 2018
 * @brief Insert brief information about this file purpose.
 * @copyright Copyright (C) 2018 mudita.com. Based on https://www.sqlite.org/src/doc/trunk/src/test_demovfs.c
 * @details :
 **
 ** This file implements an example of a simple VFS implementation that
 ** omits complex features often not required or not possible on embedded
 ** platforms.  Code is included to buffer writes to the journal file,
 ** which can be a significant performance improvement on some embedded
 ** platforms.
 **
 ** OVERVIEW
 **
 **   The code in this file implements a minimal SQLite VFS that can be
 **   used on Linux and other posix-like operating systems. The following
 **   system calls are used:
 **
 **    File-system: access(), unlink(), getcwd()
 **    File IO:     open(), read(), write(), fsync(), close(), fstat()
 **    Other:       sleep(), usleep(), time()
 **
 **   The following VFS features are omitted:
 **
 **     1. File locking. The user must ensure that there is at most one
 **        connection to each database when using this VFS. Multiple
 **        connections to a single shared-cache count as a single connection
 **        for the purposes of the previous statement.
 **
 **     2. The loading of dynamic extensions (shared libraries).
 **
 **     3. Temporary files. The user must configure SQLite to use in-memory
 **        temp files when using this VFS. The easiest way to do this is to
 **        compile with:
 **
 **          -DSQLITE_TEMP_STORE=3
 **
 **     4. File truncation. As of version 3.6.24, SQLite may run without
 **        a working xTruncate() call, providing the user does not configure
 **        SQLite to use "journal_mode=truncate", or use both
 **        "journal_mode=persist" and ATTACHed databases.
 **
 **   It is assumed that the system uses UNIX-like path-names. Specifically,
 **   that '/' characters are used to separate path components and that
 **   a path-name is a relative path unless it begins with a '/'. And that
 **   no UTF-8 encoded paths are greater than 512 bytes in length.
 **
 ** JOURNAL WRITE-BUFFERING
 **
 **   To commit a transaction to the database, SQLite first writes rollback
 **   information into the journal file. This usually consists of 4 steps:
 **
 **     1. The rollback information is sequentially written into the journal
 **        file, starting at the start of the file.
 **     2. The journal file is synced to disk.
 **     3. A modification is made to the first few bytes of the journal file.
 **     4. The journal file is synced to disk again.
 **
 **   Most of the data is written in step 1 using a series of calls to the
 **   VFS xWrite() method. The buffers passed to the xWrite() calls are of
 **   various sizes. For example, as of version 3.6.24, when committing a
 **   transaction that modifies 3 pages of a database file that uses 4096
 **   byte pages residing on a media with 512 byte sectors, SQLite makes
 **   eleven calls to the xWrite() method to create the rollback journal,
 **   as follows:
 **
 **             Write offset | Bytes written
 **             ----------------------------
 **                        0            512
 **                      512              4
 **                      516           4096
 **                     4612              4
 **                     4616              4
 **                     4620           4096
 **                     8716              4
 **                     8720              4
 **                     8724           4096
 **                    12820              4
 **             ++++++++++++SYNC+++++++++++
 **                        0             12
 **             ++++++++++++SYNC+++++++++++
 **
 **   On many operating systems, this is an efficient way to write to a file.
 **   However, on some embedded systems that do not cache writes in OS
 **   buffers it is much more efficient to write data in blocks that are
 **   an integer multiple of the sector-size in size and aligned at the
 **   start of a sector.
 **
 **   To work around this, the code in this file allocates a fixed size
 **   buffer of SQLITE_DEMOVFS_BUFFERSZ using sqlite3_malloc() whenever a
 **   journal file is opened. It uses the buffer to coalesce sequential
 **   writes into aligned SQLITE_DEMOVFS_BUFFERSZ blocks. When SQLite
 **   invokes the xSync() method to sync the contents of the file to disk,
 **   all accumulated data is written out, even if it does not constitute
 **   a complete block. This means the actual IO to create the rollback
 **   journal for the example transaction above is this:
 **
 **             Write offset | Bytes written
 **             ----------------------------
 **                        0           8192
 **                     8192           4632
 **             ++++++++++++SYNC+++++++++++
 **                        0             12
 **             ++++++++++++SYNC+++++++++++
 **
 **   Much more efficient if the underlying OS is not caching write
 **   operations.
 */

#if !defined(SQLITE_TEST) || SQLITE_OS_UNIX

#include "../../database/sqlite3.h"

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <sys/file.h>
#include <sys/param.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#include "vfs.hpp"
#include "config.h"

/*
 ** Size of the write buffer used by journal files in bytes.
 */
#ifndef SQLITE_ECOPHONEVFS_BUFFERSZ
# define SQLITE_ECOPHONEVFS_BUFFERSZ 8192
#endif

/*
 ** The maximum pathname length supported by this VFS.
 */
#define MAXPATHNAME 512

#define UNUSED(x) ((void)(x))

/*
 ** When using this VFS, the sqlite3_file* handles that SQLite uses are
 ** actually pointers to instances of type EcophoneFile.
 */
typedef struct EcophoneFile EcophoneFile;
struct EcophoneFile {
    sqlite3_file base;              /* Base class. Must be first. */
    vfs::FILE *fd;                         /* File descriptor */

    char *aBuffer;                  /* Pointer to malloc'd buffer */
    int nBuffer;                    /* Valid bytes of data in zBuffer */
    sqlite3_int64 iBufferOfst;      /* Offset in file of zBuffer[0] */
};

/*
 ** Write directly to the file passed as the first argument. Even if the
 ** file has a write-buffer (EcophoneFile.aBuffer), ignore it.
 */
static int ecophoneDirectWrite(
        EcophoneFile *p,                    /* File handle */
        const void *zBuf,               /* Buffer containing data to write */
        int iAmt,                       /* Size of data to write in bytes */
        sqlite_int64 iOfst              /* File offset to write to */
){
    size_t nWrite;                  /* Return value from write() */

    //vfs_fseek doesn't like offset to be > file size
    if (iOfst >= p->fd->ulFileSize)
        iOfst = p->fd->ulFileSize;

    if(vfs.fseek( p->fd, iOfst, SEEK_SET ) != 0){
        return SQLITE_IOERR_WRITE;
    }

    nWrite = vfs.fwrite( zBuf, 1, iAmt, p->fd);
    if( (int)nWrite!=iAmt ){
        return SQLITE_IOERR_WRITE;
    }

    return SQLITE_OK;
}

/*
 ** Flush the contents of the EcophoneFile.aBuffer buffer to disk. This is a
 ** no-op if this particular file does not have a buffer (i.e. it is not
 ** a journal file) or if the buffer is currently empty.
 */
static int ecophoneFlushBuffer(EcophoneFile *p){
    int rc = SQLITE_OK;
    if( p->nBuffer ){
        rc = ecophoneDirectWrite(p, p->aBuffer, p->nBuffer, p->iBufferOfst);
        p->nBuffer = 0;
    }
    return rc;
}

/*
 ** Close a file.
 */
static int ecophoneClose(sqlite3_file *pFile){
    int rc;
    EcophoneFile *p = (EcophoneFile*)pFile;
    rc = ecophoneFlushBuffer(p);
    sqlite3_free(p->aBuffer);

    vfs.fclose(p->fd);
    return rc;
}

/*
 ** Read data from a file.
 */
static int ecophoneRead(
        sqlite3_file *pFile,
        void *zBuf,
        int iAmt,
        sqlite_int64 iOfst
){
    EcophoneFile *p = (EcophoneFile*)pFile;
    int nRead;                      /* Return value from read() */
    int rc;                         /* Return code from ecophoneFlushBuffer() */

    /* Flush any data in the write buffer to disk in case this operation
     ** is trying to read data the file-region currently cached in the buffer.
     ** It would be possible to detect this case and possibly save an
     ** unnecessary write here, but in practice SQLite will rarely read from
     ** a journal file when there is data cached in the write-buffer.
     */
    rc = ecophoneFlushBuffer(p);
    if( rc!=SQLITE_OK ){
        return rc;
    }

    //vfs_fseek returns error if desired file position is > file size. To mimic lseek desired position need to be truncated
    if (p->fd != NULL) {
    	if (iOfst >= p->fd->ulFileSize)
    		iOfst = p->fd->ulFileSize;
    }

    if( vfs.fseek( p->fd, iOfst, FF_SEEK_SET ) != 0 ){
        return SQLITE_IOERR_READ;
    }

    nRead = vfs.fread( zBuf, 1, iAmt, p->fd );

    if( nRead==iAmt ){
        return SQLITE_OK;
    }else if( nRead>=0 ){
        return SQLITE_IOERR_SHORT_READ;
    }

    return SQLITE_IOERR_READ;
}

/*
 ** Write data to a crash-file.
 */
static int ecophoneWrite(
        sqlite3_file *pFile,
        const void *zBuf,
        int iAmt,
        sqlite_int64 iOfst
){
    EcophoneFile *p = (EcophoneFile*)pFile;

    if( p->aBuffer ){
        char *z = (char *)zBuf;       /* Pointer to remaining data to write */
        int n = iAmt;                 /* Number of bytes at z */
        sqlite3_int64 i = iOfst;      /* File offset to write to */

        while( n>0 ){
            int nCopy;                  /* Number of bytes to copy into buffer */

            /* If the buffer is full, or if this data is not being written directly
             ** following the data already buffered, flush the buffer. Flushing
             ** the buffer is a no-op if it is empty.
             */
            if( p->nBuffer==SQLITE_ECOPHONEVFS_BUFFERSZ || p->iBufferOfst+p->nBuffer!=i ){
                int rc = ecophoneFlushBuffer(p);
                if( rc!=SQLITE_OK ){
                    return rc;
                }
            }
            assert( p->nBuffer==0 || p->iBufferOfst+p->nBuffer==i );
            p->iBufferOfst = i - p->nBuffer;

            /* Copy as much data as possible into the buffer. */
            nCopy = SQLITE_ECOPHONEVFS_BUFFERSZ - p->nBuffer;
            if( nCopy>n ){
                nCopy = n;
            }
            memcpy(&p->aBuffer[p->nBuffer], z, nCopy);
            p->nBuffer += nCopy;

            n -= nCopy;
            i += nCopy;
            z += nCopy;
        }
    }else{
        return ecophoneDirectWrite(p, zBuf, iAmt, iOfst);
    }

    return SQLITE_OK;
}

/*
 ** Truncate a file. This is a no-op for this VFS (see header comments at
 ** the top of the file).
 */
static int ecophoneTruncate(sqlite3_file *pFile, sqlite_int64 size){
#if 0
    if( ftruncate(((EcophoneFile *)pFile)->fd, size) ) return SQLITE_IOERR_TRUNCATE;
#else
    UNUSED(pFile);
    UNUSED(size);
#endif
    return SQLITE_OK;
}

/*
 ** Sync the contents of the file to the persistent media.
 */
static int ecophoneSync(sqlite3_file *pFile, int flags){
    EcophoneFile *p = (EcophoneFile*)pFile;
    int rc;

    UNUSED(flags);

    rc = ecophoneFlushBuffer(p);
    if( rc!=SQLITE_OK ){
        return rc;
    }

    //rc = fsync(p->fd);  //FF doesn't have this function
    rc = SQLITE_OK;
    return (rc==0 ? SQLITE_OK : SQLITE_IOERR_FSYNC);
}

/*
 ** Write the size of the file in bytes to *pSize.
 */
static int ecophoneFileSize(sqlite3_file *pFile, sqlite_int64 *pSize){
    EcophoneFile *p = (EcophoneFile*)pFile;
    int rc;                         /* Return code from fstat() call */

    /* Flush the contents of the buffer to disk. As with the flush in the
     ** ecophoneRead() method, it would be possible to avoid this and save a write
     ** here and there. But in practice this comes up so infrequently it is
     ** not worth the trouble.
     */
    rc = ecophoneFlushBuffer(p);
    if( rc!=SQLITE_OK ){
        return rc;
    }

    *pSize = vfs.filelength( p->fd );

    return SQLITE_OK;
}

/*
 ** Locking functions. The xLock() and xUnlock() methods are both no-ops.
 ** The xCheckReservedLock() always indicates that no other process holds
 ** a reserved lock on the database file. This ensures that if a hot-journal
 ** file is found in the file-system it is rolled back.
 */
static int ecophoneLock(sqlite3_file *pFile, int eLock){
    UNUSED(pFile);
    UNUSED(eLock);
    return SQLITE_OK;
}
static int ecophoneUnlock(sqlite3_file *pFile, int eLock){
    UNUSED(pFile);
    UNUSED(eLock);
    return SQLITE_OK;
}
static int ecophoneCheckReservedLock(sqlite3_file *pFile, int *pResOut){
    UNUSED(pFile);
    *pResOut = 0;
    return SQLITE_OK;
}

/*
 ** No xFileControl() verbs are implemented by this VFS.
 */
static int ecophoneFileControl(sqlite3_file *pFile, int op, void *pArg){
    UNUSED(pFile);
    UNUSED(op);
    UNUSED(pArg);
    return SQLITE_OK;
}

/*
 ** The xSectorSize() and xDeviceCharacteristics() methods. These two
 ** may return special values allowing SQLite to optimize file-system
 ** access to some extent. But it is also safe to simply return 0.
 */
static int ecophoneSectorSize(sqlite3_file *pFile){
    UNUSED(pFile);
    return 0;
}
static int ecophoneDeviceCharacteristics(sqlite3_file *pFile){
    UNUSED(pFile);
    return 0;
}

/*
 ** Query the file-system to see if the named file exists, is readable or
 ** is both readable and writable.
 */
static int ecophoneAccess(
        sqlite3_vfs *pVfs,
        const char *zPath,
        int flags,
        int *pResOut
){
    vfs::FILE *fd;
    UNUSED(pVfs);

    assert( flags==SQLITE_ACCESS_EXISTS       /* access(zPath, F_OK) */
            || flags==SQLITE_ACCESS_READ         /* access(zPath, R_OK) */
            || flags==SQLITE_ACCESS_READWRITE    /* access(zPath, R_OK|W_OK) */
    );

    fd = vfs.fopen(zPath, (const char*)"r" );
    if( fd != NULL)
    {
        if (pResOut) *pResOut = flags;
        vfs.fclose(fd);
    }
    else
        if (pResOut) *pResOut = 0;
    return SQLITE_OK;
}

/*
 ** Open a file handle.
 */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
static int ecophoneOpen(
        sqlite3_vfs *pVfs,              /* VFS */
        const char *zName,              /* File to open, or 0 for a temp file */
        sqlite3_file *pFile,            /* Pointer to EcophoneFile struct to populate */
        int flags,                      /* Input SQLITE_OPEN_XXX flags */
        int *pOutFlags                  /* Output SQLITE_OPEN_XXX flags (or NULL) */
){
    UNUSED(pVfs);

    static const sqlite3_io_methods ecophoneio = {
            1,                            /* iVersion */
            ecophoneClose,                    /* xClose */
            ecophoneRead,                     /* xRead */
            ecophoneWrite,                    /* xWrite */
            ecophoneTruncate,                 /* xTruncate */
            ecophoneSync,                     /* xSync */
            ecophoneFileSize,                 /* xFileSize */
            ecophoneLock,                     /* xLock */
            ecophoneUnlock,                   /* xUnlock */
            ecophoneCheckReservedLock,        /* xCheckReservedLock */
            ecophoneFileControl,              /* xFileControl */
            ecophoneSectorSize,               /* xSectorSize */
            ecophoneDeviceCharacteristics     /* xDeviceCharacteristics */
    };

    EcophoneFile *p = (EcophoneFile*)pFile; /* Populate this structure */
    char *aBuf = 0;
    FF_DirHandler_t xHandler;
    FF_Error_t xError;

    if( zName==0 ){
        return SQLITE_IOERR;
    }

    if( flags&SQLITE_OPEN_MAIN_JOURNAL ){
        aBuf = (char *)sqlite3_malloc(SQLITE_ECOPHONEVFS_BUFFERSZ);
        if( !aBuf ){
            return SQLITE_NOMEM;
        }
    }

    memset(p, 0, sizeof(EcophoneFile));

    /* Look-up the I/O manager for the file system. */
    if( FF_FS_Find( zName, &xHandler ) != pdFALSE )
    {
        //not using vfs_fopen because it lacks proper flag combination
        if( flags&SQLITE_OPEN_READONLY ) p->fd = FF_Open( xHandler.pxManager, xHandler.pcPath, FF_MODE_READ, &xError );
        else if (flags&SQLITE_OPEN_READWRITE) p->fd = FF_Open( xHandler.pxManager, xHandler.pcPath, FF_MODE_READ | FF_MODE_WRITE | FF_MODE_CREATE, &xError );
    }
    else
    {
        sqlite3_free(aBuf);
        return SQLITE_CANTOPEN;
    }

    p->aBuffer = aBuf;

    if( pOutFlags ){
        *pOutFlags = flags;
    }
    p->base.pMethods = &ecophoneio;
    return SQLITE_OK;
}

/*
 ** Delete the file identified by argument zPath. If the dirSync parameter
 ** is non-zero, then ensure the file-system modification to delete the
 ** file has been synced to disk before returning.
 */
static int ecophoneDelete(sqlite3_vfs *pVfs, const char *zPath, int dirSync){
    UNUSED(pVfs);
    int rc;                         /* Return code */

    rc = ff_remove( zPath );
    if( rc!=0 /*&& errno==ENOENT*/ ) return SQLITE_OK;

    if( rc==0 && dirSync ){
        vfs::FILE *dfd;                      /* File descriptor open on directory */
        int i;                        /* Iterator variable */
        char zDir[MAXPATHNAME+1];     /* Name of directory containing file zPath */

        /* Figure out the directory name from the path of the file deleted. */
        sqlite3_snprintf(MAXPATHNAME, zDir, "%s", zPath);
        zDir[MAXPATHNAME] = '\0';
        for(i=strlen(zDir); i>1 && zDir[i]!='/'; i++);
        zDir[i] = '\0';

        /* Open a file-descriptor on the directory. Sync. Close. */
        dfd = vfs.fopen( zDir, "D" );
        if( dfd == NULL ){
            rc = -1;
        }else{
            rc = SQLITE_OK;
            vfs.fclose(dfd);
        }
    }
    return (rc==0 ? SQLITE_OK : SQLITE_IOERR_DELETE);
}

/*
 ** Argument zPath points to a nul-terminated string containing a file path.
 ** If zPath is an absolute path, then it is copied as is into the output
 ** buffer. Otherwise, if it is a relative path, then the equivalent full
 ** path is written to the output buffer.
 **
 ** This function assumes that paths are UNIX style. Specifically, that:
 **
 **   1. Path components are separated by a '/'. and
 **   2. Full paths begin with a '/' character.
 */
static int ecophoneFullPathname(
        sqlite3_vfs *pVfs,              /* VFS */
        const char *zPath,              /* Input path (possibly a relative path) */
        int nPathOut,                   /* Size of output buffer in bytes */
        char *zPathOut                  /* Pointer to output buffer */
){
    UNUSED(pVfs);

    char zDir[MAXPATHNAME+1];
    if( zPath[0]=='/' ){
        zDir[0] = '\0';
    }else{
        if( ff_getcwd(zDir, sizeof(zDir))==0 ) return SQLITE_IOERR;
    }
    zDir[MAXPATHNAME] = '\0';

    sqlite3_snprintf(nPathOut, zPathOut, "%s%s", zDir, zPath);
    zPathOut[nPathOut-1] = '\0';

    return SQLITE_OK;
}

/*
 ** The following four VFS methods:
 **
 **   xDlOpen
 **   xDlError
 **   xDlSym
 **   xDlClose
 **
 ** are supposed to implement the functionality needed by SQLite to load
 ** extensions compiled as shared objects. This simple VFS does not support
 ** this functionality, so the following functions are no-ops.
 */
static void *ecophoneDlOpen(sqlite3_vfs *pVfs, const char *zPath){
    UNUSED(pVfs);
    UNUSED(zPath);
    return 0;
}
static void ecophoneDlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg){
    UNUSED(pVfs);
    sqlite3_snprintf(nByte, zErrMsg, "Loadable extensions are not supported");
    zErrMsg[nByte-1] = '\0';
}
static void (*ecophoneDlSym(sqlite3_vfs *pVfs, void *pH, const char *z))(void){
    UNUSED(pVfs);
    UNUSED(pH);
    UNUSED(z);
    return 0;
}
static void ecophoneDlClose(sqlite3_vfs *pVfs, void *pHandle){
    UNUSED(pVfs);
    UNUSED(pHandle);
    return;
}

/*
 ** Parameter zByte points to a buffer nByte bytes in size. Populate this
 ** buffer with pseudo-random data.
 */
static int ecophoneRandomness(sqlite3_vfs *pVfs, int nByte, char *zByte){
    UNUSED(pVfs);
    UNUSED(nByte);
    UNUSED(zByte);
    return SQLITE_OK;
}

/*
 ** Sleep for at least nMicro microseconds. Return the (approximate) number
 ** of microseconds slept for.
 */
static int ecophoneSleep(sqlite3_vfs *pVfs, int nMicro){
    UNUSED(pVfs);

    const TickType_t xDelay = nMicro / 1000 / portTICK_PERIOD_MS;
    vTaskDelay( xDelay );

    return nMicro;
}

/*
 ** Set *pTime to the current UTC time expressed as a Julian day. Return
 ** SQLITE_OK if successful, or an error code otherwise.
 **
 **   http://en.wikipedia.org/wiki/Julian_day
 **
 ** This implementation is not very good. The current time is rounded to
 ** an integer number of seconds. Also, assuming time_t is a signed 32-bit
 ** value, it will stop working some time in the year 2038 AD (the so-called
 ** "year 2038" problem that afflicts systems that store time this way).
 */
static int ecophoneCurrentTime(sqlite3_vfs *pVfs, double *pTime){

    time_t t = time(0);
    *pTime = t/86400.0 + 2440587.5;
    return SQLITE_OK;
}

/*
 ** This function returns a pointer to the VFS implemented in this file.
 ** To make the VFS available to SQLite:
 **
 **   sqlite3_vfs_register(sqlite3_ecophonevfs(), 0);
 */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
sqlite3_vfs *sqlite3_ecophonevfs(void){
    static sqlite3_vfs ecophonevfs = {
            1,                            /* iVersion */
            sizeof(EcophoneFile),             /* szOsFile */
            MAXPATHNAME,                  /* mxPathname */
            0,                            /* pNext */
            "ecophone",                       /* zName */
            0,                            /* pAppData */
            ecophoneOpen,                     /* xOpen */
            ecophoneDelete,                   /* xDelete */
            ecophoneAccess,                   /* xAccess */
            ecophoneFullPathname,             /* xFullPathname */
            ecophoneDlOpen,                   /* xDlOpen */
            ecophoneDlError,                  /* xDlError */
            ecophoneDlSym,                    /* xDlSym */
            ecophoneDlClose,                  /* xDlClose */
            ecophoneRandomness,               /* xRandomness */
            ecophoneSleep,                    /* xSleep */
            ecophoneCurrentTime,              /* xCurrentTime */
    };
    return &ecophonevfs;
}

#endif /* !defined(SQLITE_TEST) || SQLITE_OS_UNIX */
