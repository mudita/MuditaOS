// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once

/* These are the fs-independent mount-flags: up to 16 flags are
   supported  */
enum
{
    MS_RDONLY      = 1,    /* Mount read-only.  */
    MS_NOEXEC      = 8,    /* Disallow program execution.  */
    MS_SYNCHRONOUS = 16,   /* Writes are synced at once.  */
    MS_REMOUNT     = 32,   /* Alter flags of a mounted FS.  */
    MS_MANDLOCK    = 64,   /* Allow mandatory locks on an FS.  */
    MS_DIRSYNC     = 128,  /* Directory modifications are synchronous.  */
    MS_NOATIME     = 1024, /* Do not update access times.  */
    MS_NODIRATIME  = 2048, /* Do not update directory access times.  */
    MS_BIND        = 4096, /* Bind directory at different place.  */
};

#ifdef __cplusplus
extern "C"
{
#endif

    /* Mount a filesystem.  */
    int mount(
        const char *special_file, const char *dir, const char *fstype, unsigned long int rwflag, const void *data);

    /* Unmount a filesystem.  */
    int umount(const char *special_file);

#ifdef __cplusplus
}
#endif
