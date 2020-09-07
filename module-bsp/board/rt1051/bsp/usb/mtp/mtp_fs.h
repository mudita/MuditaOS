#ifndef _MTP_FS_H
#define _MTP_FS_H

struct mtp_fs;

extern const struct mtp_storage_api simple_fs_api;

struct mtp_fs* mtp_fs_alloc(void *disk);

#endif /* _MTP_FS_H */
