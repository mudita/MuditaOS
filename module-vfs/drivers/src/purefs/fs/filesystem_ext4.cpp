// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <memory>
#include <purefs/fs/drivers/filesystem_ext4.hpp>
#include <purefs/fs/drivers/mount_point_ext4.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_handle.hpp>
#include <purefs/fs/drivers/file_handle_ext4.hpp>
#include <purefs/fs/drivers/directory_handle_ext4.hpp>
#include <lwext4/ext4_bdev.hpp>
#include <log/log.hpp>
#include <ext4.h>
#include <ext4_inode.h>
#include <ext4_super.h>

#include <climits>
#include <syslimits.h>
#include <sys/statvfs.h>
#include <errno.h>
#include <cstring>
#include <algorithm>
#include <sys/stat.h>

namespace purefs::fs::drivers
{
    namespace
    {
        template <typename T, typename... Args>
        auto invoke_efs(filesystem_ext4::fsfile zfil, T efs_fun, Args &&...args)
        {
            auto vfile = std::dynamic_pointer_cast<file_handle_ext4>(zfil);
            if (!vfile) {
                LOG_ERROR("Non ext4 filesystem file pointer");
                return -EBADF;
            }
            auto mntp = std::static_pointer_cast<mount_point_ext4>(vfile->mntpoint());
            if (!mntp) {
                LOG_ERROR("Non ext4 mount point");
                return -EBADF;
            }
            ext4_locker _lck(mntp);
            auto err = efs_fun(vfile->filp(), std::forward<Args>(args)...);
            return -err;
        }
        template <typename T>
        auto invoke_efs(filesystem_ext4::fsmount mnt, T efs_fun, std::string_view oldpath, std::string_view newpath)
        {
            auto mntp = std::dynamic_pointer_cast<mount_point_ext4>(mnt);
            if (!mntp) {
                LOG_ERROR("Non ext4 mount point");
                return -EBADF;
            }
            const auto native_old = mntp->native_path(oldpath);
            const auto native_new = mntp->native_path(newpath);
            ext4_locker _lck(mntp);
            auto err = efs_fun(native_old.c_str(), native_new.c_str());
            return -err;
        }
        template <typename T, typename... Args>
        auto invoke_efs(filesystem_ext4::fsmount fmnt, T efs_fun, std::string_view path, Args &&...args)
        {
            auto mntp = std::dynamic_pointer_cast<mount_point_ext4>(fmnt);
            if (!mntp) {
                LOG_ERROR("Non ext4 mount point");
                return -EBADF;
            }
            const auto native_path = mntp->native_path(path);
            ext4_locker _lck(mntp);
            auto err = efs_fun(native_path.c_str(), std::forward<Args>(args)...);
            return -err;
        }
        template <typename T, typename... Args>
        auto invoke_efs(filesystem_ext4::fsdir zdir, T lfs_fun, Args &&...args)
        {
            auto vdir = std::dynamic_pointer_cast<directory_handle_ext4>(zdir);
            if (!vdir) {
                LOG_ERROR("Non ext4 filesystem directory pointer");
                return -EBADF;
            }
            auto mntp = std::static_pointer_cast<mount_point_ext4>(vdir->mntpoint());
            if (!mntp) {
                LOG_ERROR("Non ext4 mount point");
                return -EBADF;
            }
            ext4_locker _lck(mntp);
            auto err = lfs_fun(vdir->dirp(), std::forward<Args>(args)...);
            return err;
        }
        inline auto ino_to_st_mode(int dtype)
        {
            switch (dtype) {
            case EXT4_DE_REG_FILE:
                return S_IFREG;
            case EXT4_DE_DIR:
                return S_IFDIR;
            case EXT4_DE_CHRDEV:
                return S_IFCHR;
            case EXT4_DE_BLKDEV:
                return S_IFBLK;
            case EXT4_DE_FIFO:
                return S_IFIFO;
            case EXT4_DE_SOCK:
                return S_IFSOCK;
            default:
                return 0;
            }
        }

    } // namespace
    auto filesystem_ext4::mount_prealloc(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                                         std::string_view path,
                                         unsigned flags) -> fsmount
    {
        return std::make_shared<mount_point_ext4>(diskh, path, flags, shared_from_this());
    }

    auto filesystem_ext4::mount(fsmount mnt, const void *data) noexcept -> int
    {
        auto disk = mnt->disk();
        if (!disk) {
            return -EIO;
        }
        auto vmnt = std::dynamic_pointer_cast<mount_point_ext4>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non ext4 mount point");
            return -EIO;
        }
        ext4_locker _lck(vmnt);
        auto [bd, err] = ext4::internal::append_volume(disk_mngr(), disk);
        if (err) {
            LOG_ERROR("Unable to append volume err: %i", err);
            return err;
        }
        /** If verbosed lwext4 debug is required please uncomment
         * this line. It may cause to print a lot of messages
         * especially when the ext4 journal is recovered
         * on the log output device so it is disabled by default
         */
        // ext4_dmask_set(DEBUG_ALL);
        err = ext4_device_register(bd, disk->name().c_str());
        if (err) {
            LOG_ERROR("Unable to register device with err: %i", err);
            ext4::internal::remove_volume(bd);
            return -err;
        }
        const auto mnt_path = vmnt->mount_path();
        // Mount
        err = ext4_mount(disk->name().c_str(), mnt_path.c_str(), 0);
        if (err) {
            LOG_ERROR("Unable to mount ext4 errno %i", err);
            ext4_device_unregister(disk->name().c_str());
            ext4::internal::remove_volume(bd);
            return -err;
        }
        // Start ext4 recover
        err = ext4_recover(mnt_path.c_str());
        if (err) {
            LOG_ERROR("Ext4 recover failed errno %i", err);
            ext4_umount(mnt_path.c_str());
            ext4_device_unregister(disk->name().c_str());
            ext4::internal::remove_volume(bd);
            return -err;
        }
        // Start journaling
        err = ext4_journal_start(mnt_path.c_str());
        if (err) {
            LOG_WARN("Unable to start journaling errno %i", err);
        }
        err = ext4_block_cache_write_back(bd, true);
        if (err) {
            LOG_ERROR("Unable to switch to write back mode errno %i", err);
            ext4_umount(mnt_path.c_str());
            ext4_device_unregister(disk->name().c_str());
            ext4::internal::remove_volume(bd);
            return -err;
        }
        filesystem_operations::mount(mnt, data);
        vmnt->block_dev(bd);
        return err;
    }

    // Unmount filesystem
    auto filesystem_ext4::umount(fsmount mnt) noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_ext4>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non ext4 mount point");
            return -EIO;
        }
        ext4_locker _lck(vmnt);
        auto err = ext4_cache_write_back(mnt->mount_path().c_str(), false);
        if (err) {
            LOG_WARN("Unable to disable cache wb errno %i", err);
            err = 0;
        }
        err = ext4_journal_stop(mnt->mount_path().c_str());
        if (err) {
            LOG_WARN("Unable to stop ext4 journal %i", err);
            err = 0;
        }
        err = ext4_umount(mnt->mount_path().c_str());
        if (err) {
            LOG_ERROR("Unable to umount device");
            return -err;
        }
        //! NOTE: Bug in the lib it always return ENOENT
        ext4_device_unregister(vmnt->disk()->name().c_str());
        err = ext4::internal::remove_volume(vmnt->block_dev());
        if (err) {
            LOG_ERROR("Remove volume error %i", err);
            return err;
        }
        filesystem_operations::umount(mnt);
        return err;
    }

    // Stat the filesystem
    auto filesystem_ext4::stat_vfs(fsmount mnt, std::string_view path, struct statvfs &stat) const noexcept -> int
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_ext4>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non ext4 mount point");
            return -EIO;
        }
        ext4_locker _lck(vmnt);
        ext4_mount_stats estats;
        auto err = ext4_mount_point_stats(vmnt->mount_path().c_str(), &estats);
        if (err) {
            LOG_ERROR("Mount point stats error %i", err);
            return -err;
        }
        std::memset(&stat, 0, sizeof stat);
        stat.f_bsize   = estats.block_size;
        stat.f_frsize  = estats.block_size;
        stat.f_blocks  = estats.blocks_count;
        stat.f_bfree   = estats.free_blocks_count;
        stat.f_bavail  = stat.f_bfree;
        stat.f_files   = estats.inodes_count;
        stat.f_ffree   = estats.free_inodes_count;
        stat.f_favail  = stat.f_ffree;
        stat.f_flag    = vmnt->flags();
        stat.f_namemax = PATH_MAX;
        return err;
    }

    auto filesystem_ext4::open(fsmount mnt, std::string_view path, int flags, int mode) noexcept -> fsfile
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_ext4>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non ext4 mount point");
            return nullptr;
        }
        ext4_locker _lck(vmnt);
        const auto fspath = vmnt->native_path(path);
        auto filp         = std::make_shared<file_handle_ext4>(mnt, fspath, flags);
        auto err          = ext4_fopen2(filp->filp(), fspath.c_str(), flags);
        if (err == EOK) {
            ext4_atime_set(fspath.c_str(), time(nullptr));
        }
        filp->error(-err);
        return filp;
    }

    auto filesystem_ext4::close(fsfile zfile) noexcept -> int
    {
        return -invoke_efs(zfile, ::ext4_fclose);
    }

    auto filesystem_ext4::write(fsfile zfile, const char *ptr, size_t len) noexcept -> ssize_t
    {
        size_t n_written;
        const auto err = invoke_efs(zfile, ::ext4_fwrite, ptr, len, &n_written);
        if (err == EOK) {
            const auto vfile     = std::dynamic_pointer_cast<file_handle_ext4>(zfile);
            const auto vmnt      = std::dynamic_pointer_cast<mount_point_ext4>(vfile->mntpoint());
            const auto timestamp = time(nullptr);

            ext4_locker _lck(vmnt);
            ext4_ctime_set(vfile->open_path().c_str(), timestamp);
            ext4_mtime_set(vfile->open_path().c_str(), timestamp);
        }
        return (err) ? (-err) : (n_written);
    }

    auto filesystem_ext4::read(fsfile zfile, char *ptr, size_t len) noexcept -> ssize_t
    {
        size_t n_read;
        auto err = invoke_efs(zfile, ::ext4_fread, ptr, len, &n_read);
        return (err) ? (-err) : (n_read);
    }

    auto filesystem_ext4::seek(fsfile zfile, off_t pos, int dir) noexcept -> off_t
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_ext4>(zfile);
        if (!vfile) {
            LOG_ERROR("Non ext4 filesystem file pointer");
            return -EBADF;
        }
        auto mntp = std::static_pointer_cast<mount_point_ext4>(vfile->mntpoint());
        if (!mntp) {
            LOG_ERROR("Non ext4 mount point");
            return -EBADF;
        }
        ext4_locker _lck(mntp);
        auto err = ext4_fseek(vfile->filp(), pos, dir);
        if (err) {
            return -err;
        }
        auto rpos = ext4_ftell(vfile->filp());
        return rpos;
    }

    auto filesystem_ext4::stat(const char *mount_point, const char *path, struct stat *st, bool ro) noexcept -> int
    {
        uint32_t inonum;
        ext4_inode ino;
        ext4_sblock *sb;
        auto err = ext4_raw_inode_fill(path, &inonum, &ino);
        if (err) {
            return -err;
        }
        err = ext4_get_sblock(mount_point, &sb);
        if (err) {
            return -err;
        }
        std::memset(st, 0, sizeof(*st));
        st->st_ino       = inonum;
        const auto btype = ext4_inode_type(sb, &ino);
        st->st_mode      = ext4_inode_get_mode(sb, &ino) | ino_to_st_mode(btype);
        if (ro) {
            st->st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
        }
        // Update file type
        st->st_nlink   = ext4_inode_get_links_cnt(&ino);
        st->st_uid     = ext4_inode_get_uid(&ino);
        st->st_gid     = ext4_inode_get_gid(&ino);
        st->st_blocks  = ext4_inode_get_blocks_count(sb, &ino);
        st->st_size    = ext4_inode_get_size(sb, &ino);
        st->st_blksize = ext4_sb_get_block_size(sb);
        st->st_dev     = ext4_inode_get_dev(&ino);
#ifdef __APPLE__
        st->st_atimespec.tv_sec = ext4_inode_get_access_time(&ino);
        st->st_ctimespec.tv_sec = ext4_inode_get_change_inode_time(&ino);
        st->st_mtimespec.tv_sec = ext4_inode_get_modif_time(&ino);
#else
        st->st_atim.tv_sec = ext4_inode_get_access_time(&ino);
        st->st_ctim.tv_sec = ext4_inode_get_change_inode_time(&ino);
        st->st_mtim.tv_sec = ext4_inode_get_modif_time(&ino);
#endif
        return err;
    }

    auto filesystem_ext4::fstat(fsfile zfile, struct stat &st) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_ext4>(zfile);
        if (!vfile) {
            LOG_ERROR("Non ext4 filesystem file pointer");
            return -EBADF;
        }
        auto mntp = std::static_pointer_cast<mount_point_ext4>(vfile->mntpoint());
        if (!mntp) {
            LOG_ERROR("Non ext4 mount point");
            return -EBADF;
        }
        const auto path = vfile->open_path();
        ext4_locker _lck(mntp);
        return stat(mntp->mount_path().c_str(), vfile->open_path().c_str(), &st, zfile->mntpoint()->is_ro());
    }

    auto filesystem_ext4::stat(fsmount mnt, std::string_view file, struct stat &st) noexcept -> int
    {
        auto mntp = std::dynamic_pointer_cast<mount_point_ext4>(mnt);
        if (!mntp) {
            LOG_ERROR("Non ext4 mount point");
            return -EBADF;
        }
        // NOTE: mod path fix for lwext4 open root dir
        auto npath = mntp->native_path(file);
        if (npath.empty()) {
            npath = mntp->mount_path();
        }
        ext4_locker _lck(mntp);
        return stat(mntp->mount_path().c_str(), npath.c_str(), &st, mntp->is_ro());
    }

    auto filesystem_ext4::link(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept -> int
    {
        return invoke_efs(mnt, ::ext4_flink, existing, newlink);
    }

    auto filesystem_ext4::symlink(fsmount mnt, std::string_view existing, std::string_view newlink) noexcept -> int
    {
        return invoke_efs(mnt, ::ext4_fsymlink, existing, newlink);
    }

    auto filesystem_ext4::unlink(fsmount mnt, std::string_view name) noexcept -> int
    {
        return invoke_efs(
            mnt,
            [](const char *path) {
                if (ext4_inode_exist(path, EXT4_DE_DIR) == 0) {
                    LOG_WARN("rmdir syscall instead of unlink is recommended for remove directory");
                    return -ext4_dir_rm(path);
                }
                else {
                    return -ext4_fremove(path);
                }
            },
            name);
    }

    auto filesystem_ext4::rmdir(fsmount mnt, std::string_view name) noexcept -> int
    {
        return invoke_efs(mnt, ::ext4_dir_rm, name);
    }

    auto filesystem_ext4::rename(fsmount mnt, std::string_view oldname, std::string_view newname) noexcept -> int
    {
        return invoke_efs(mnt, ::ext4_frename, oldname, newname);
    }

    auto filesystem_ext4::mkdir(fsmount mnt, std::string_view path, int mode) noexcept -> int
    {
        return invoke_efs(mnt, ::ext4_dir_mk, path);
    }

    auto filesystem_ext4::diropen(fsmount mnt, std::string_view path) noexcept -> fsdir
    {
        auto vmnt = std::dynamic_pointer_cast<mount_point_ext4>(mnt);
        if (!vmnt) {
            LOG_ERROR("Non ext4 mount point");
            return nullptr;
        }
        // NOTE: mod path fix for lwext4 open root dir
        auto fspath = vmnt->native_path(path);
        if (fspath.empty()) {
            fspath = vmnt->mount_path();
        }
        const auto dirp = std::make_shared<directory_handle_ext4>(mnt, 0);
        ext4_locker _lck(vmnt);
        const auto lret = ext4_dir_open(dirp->dirp(), fspath.c_str());
        dirp->error(-lret);
        return dirp;
    }

    auto filesystem_ext4::dirreset(fsdir dirstate) noexcept -> int
    {
        return invoke_efs(dirstate, [](auto arg) {
            ::ext4_dir_entry_rewind(arg);
            return 0;
        });
    }

    auto filesystem_ext4::dirnext(fsdir dirstate, std::string &filename, struct stat &st) -> int
    {
        const ext4_direntry *dentry{};
        const auto err = invoke_efs(dirstate, [&dentry](auto arg) {
            dentry = ext4_dir_entry_next(arg);
            return dentry ? 0 : -ENODATA;
        });
        if (!err) {
            std::memset(&st, 0, sizeof(st));
            st.st_ino  = dentry->inode;
            st.st_mode = ino_to_st_mode(dentry->inode_type);
            filename   = std::string(reinterpret_cast<const char *>(dentry->name), dentry->name_length);
        }
        return err;
    }

    auto filesystem_ext4::dirclose(fsdir dirstate) noexcept -> int
    {
        return invoke_efs(dirstate, ::ext4_dir_close);
    }

    auto filesystem_ext4::isatty(fsfile zfile) noexcept -> int
    {
        return 0;
    }

    auto filesystem_ext4::chmod(fsmount mnt, std::string_view path, mode_t mode) noexcept -> int
    {
        return invoke_efs(mnt, ::ext4_mode_set, path, mode);
    }

    auto filesystem_ext4::fchmod(fsfile zfile, mode_t mode) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_ext4>(zfile);
        if (!vfile) {
            LOG_ERROR("Non ext4 filesystem file pointer");
            return -EBADF;
        }
        return invoke_efs(vfile->mntpoint(), ::ext4_mode_set, vfile->open_path().c_str(), mode);
    }

    auto filesystem_ext4::ftruncate(fsfile zfile, off_t len) noexcept -> int
    {
        return invoke_efs(
            zfile,
            [](ext4_file *file, uint64_t length) {
                int err = ext4_ftruncate(file, length);
                if (err == ENOTSUP) {
                    // NOTE: Ext4 ftruncate supports only shrinking
                    const size_t zbuf_len = 8192;
                    auto buf              = std::make_unique<char[]>(zbuf_len);
                    err                   = 0;
                    for (size_t n = 0; n < length / zbuf_len; ++n) {
                        err = ext4_fwrite(file, buf.get(), zbuf_len, nullptr);
                        if (err) {
                            err = -err;
                            break;
                        }
                    }
                    if (!err) {
                        const ssize_t remain = length % zbuf_len;
                        if (remain > 0) {
                            err = -ext4_fwrite(file, buf.get(), remain, nullptr);
                        }
                    }
                }
                return err;
            },
            len);
    }

    auto filesystem_ext4::fsync(fsfile zfile) noexcept -> int
    {
        auto vfile = std::dynamic_pointer_cast<file_handle_ext4>(zfile);
        if (!vfile) {
            LOG_ERROR("Non ext4 filesystem file pointer");
            return -EBADF;
        }
        return invoke_efs(vfile->mntpoint(), ::ext4_cache_flush, vfile->mntpoint()->mount_path().c_str());
    }

} // namespace purefs::fs::drivers
