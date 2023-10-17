// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <lfs.h>
#include "lfs_ioaccess.h"
#include "parse_partitions.h"
#include "parse_args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>

struct lfs_info_summary
{
    size_t files_added;
    size_t directories_added;
    size_t bytes_transferred;
};

static int create_dir_in_lfs(lfs_t *lfs, const char *lfs_path, bool verbose)
{
    int ret;
    if (verbose)
        fprintf(stdout, "[%s]\n", lfs_path);
    if ((ret = lfs_mkdir(lfs, lfs_path)) < 0) {
        fprintf(stderr, "can't create directory %s: error=%d\n", lfs_path, ret);
        return ret;
    }
    return 0;
}

static int create_file_in_lfs(lfs_t *lfs, const char *host_file, const char *lfs_file, bool verbose)
{
    int ret;
    if (verbose)
        fprintf(stdout, "%s\n", lfs_file);
    // Open source file
    int srcfd = open(host_file, O_RDONLY);
    if (srcfd < 0) {
        fprintf(stderr, "can't open source file %s: errno=%d (%s)\n", host_file, errno, strerror(errno));
        return -1;
    }

    // Open destination file
    lfs_file_t dstf;
    if ((ret = lfs_file_open(lfs, &dstf, lfs_file, LFS_O_WRONLY | LFS_O_CREAT)) < 0) {
        fprintf(stderr, "can't open destination file %s: error=%d\n", lfs_file, ret);
        close(srcfd);
        return ret;
    }
    do {
        char copy_buffer[16384];
        ret = read(srcfd, copy_buffer, sizeof copy_buffer);
        if (ret < 0) {
            close(srcfd);
            lfs_file_close(lfs, &dstf);
            return ret;
        }
        else if (ret == 0) {
            break;
        }
        char *lfs_wptr = copy_buffer;
        int lfs_wrleft = ret;
        do {
            int retlfs = lfs_file_write(lfs, &dstf, lfs_wptr, lfs_wrleft);
            if (retlfs <= 0) {
                close(srcfd);
                lfs_file_close(lfs, &dstf);
                fprintf(stderr, "can't write to destination file %s: error=%d\n", lfs_file, retlfs);
                return retlfs;
            }
            else {
                lfs_wrleft -= retlfs;
                lfs_wptr += retlfs;
            }
        } while (lfs_wrleft > 0);

    } while (ret > 0);

    // Close destination file
    ret = lfs_file_close(lfs, &dstf);
    if (ret < 0) {
        fprintf(stderr, "can't close destination file %s: error=%d\n", lfs_file, ret);
        close(srcfd);
        return ret;
    }

    // Close source file
    close(srcfd);
    return ret;
}

static int add_directory_to_lfs(
    lfs_t *lfs, const char *host_path, const char *lfs_path, struct lfs_info_summary *summary, bool verbose)
{
    DIR *dir;
    struct dirent *ent;
    char lfs_curr_path[PATH_MAX];
    char host_curr_path[PATH_MAX];
    int err = 0;
    dir     = opendir(host_path);
    if (dir) {
        while ((ent = readdir(dir))) {
            // Skip . and .. directories
            if ((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") != 0)) {
                // Update the current path
                strcpy(lfs_curr_path, lfs_path);
                strcat(lfs_curr_path, "/");
                strcat(lfs_curr_path, ent->d_name);
                // Update native current path
                strcpy(host_curr_path, host_path);
                strcat(host_curr_path, "/");
                strcat(host_curr_path, ent->d_name);

                if (ent->d_type == DT_DIR) {
                    err = create_dir_in_lfs(lfs, lfs_curr_path, verbose);
                    if (err) {
                        closedir(dir);
                        return err;
                    }
                    else {
                        summary->directories_added++;
                    }
                    err = add_directory_to_lfs(lfs, host_curr_path, lfs_curr_path, summary, verbose);
                    if (err) {
                        closedir(dir);
                        return err;
                    }
                }
                else if (ent->d_type == DT_REG) {
                    err = create_file_in_lfs(lfs, host_curr_path, lfs_curr_path, verbose);
                    if (err) {
                        closedir(dir);
                        return err;
                    }
                    else {
                        summary->files_added++;
                        struct stat statbuf;
                        if (stat(host_curr_path, &statbuf) == 0) {
                            summary->bytes_transferred += statbuf.st_size;
                        }
                    }
                }
            }
        }
        closedir(dir);
    }
    return err;
}

static int add_to_lfs(lfs_t *lfs, const char *dir, struct lfs_info_summary *summary, bool verbose)
{
    char *host_dir = canonicalize_file_name(dir);
    bool is_dir, is_file;
    off_t fsize;
    {
        struct stat stbuf;
        int err = stat(host_dir, &stbuf);
        if (err < 0) {
            free(host_dir);
            return -1;
        }
        fsize   = stbuf.st_size;
        is_dir  = stbuf.st_mode & S_IFDIR;
        is_file = stbuf.st_mode & S_IFREG;
    }
    if (!is_dir && !is_file) {
        free(host_dir);
        errno = ENOTDIR;
        return -1;
    }
    char *sep_ptr = strrchr(host_dir, '/');
    char *tgt_dir = malloc(strlen(sep_ptr + 1) + sizeof('\0') + sizeof('/'));
    if (!tgt_dir) {
        free(host_dir);
        errno = ENOMEM;
        return -1;
    }
    else {
        tgt_dir[0] = '/';
        strcpy(tgt_dir + 1, sep_ptr + 1);
    }
    int err = 0;
    if (is_dir) {
        err = create_dir_in_lfs(lfs, tgt_dir, verbose);
        if (err) {
            free(host_dir);
            free(tgt_dir);
            return err;
        }
        err = add_directory_to_lfs(lfs, host_dir, tgt_dir, summary, verbose);
        if (!err) {
            summary->directories_added++;
        }
    }
    else { // is_file
        err = create_file_in_lfs(lfs, host_dir, tgt_dir, verbose);
        if (!err) {
            summary->files_added++;
            summary->bytes_transferred += fsize;
        }
    }
    free(host_dir);
    free(tgt_dir);
    return err;
}

static void print_error(int error, const char *format, ...) __attribute__((nonnull(2)));
static void print_error(int error, const char *format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    vfprintf(stderr, format, arglist);
    va_end(arglist);

    if (error == -1) {
        char buf[1024];
        fprintf(stderr, " system_error: %s\n", strerror_r(errno, buf, sizeof buf));
    }
    else {
        fprintf(stderr, " lfs_error: %i\n", error);
    }
}

static void configure_lfs_params(struct lfs_config *lfsc, const struct littlefs_opts *opts)
    __attribute__((nonnull(1, 2)));

static void configure_lfs_params(struct lfs_config *lfsc, const struct littlefs_opts *opts)
{
    memset(lfsc, 0, sizeof *lfsc);
    lfsc->block_size     = opts->block_size;
    lfsc->read_size      = opts->read_size;
    lfsc->prog_size      = opts->prog_size;
    lfsc->lookahead_size = opts->lockahead_size;
    lfsc->cache_size     = opts->cache_size;
    lfsc->block_cycles   = opts->block_cycles;
}

int main(int argc, char **argv)
{

    int err = 0;
    struct littlefs_opts lopts;
    struct lfs_config cfg;
    struct lfs_info_summary prog_summary;
    struct lfs_ioaccess_context *ioctx = NULL;
    lfs_t lfs;
    err = parse_program_args(argc, argv, &lopts);
    if (err < 0) {
        return err;
    }
    if (lopts.mode == littlefs_opts_listparts) {
        size_t elems;
        struct partition *parts = find_partitions(lopts.dst_image, scan_all_partitions, &elems);
        print_partitions(parts, elems);
        free(parts);
        free(lopts.src_dirs);
        return EXIT_SUCCESS;
    }

    configure_lfs_params(&cfg, &lopts);
    memset(&prog_summary, 0, sizeof(prog_summary));
    if (lopts.mode == littlefs_opts_parts) {
        size_t elems;
        struct partition *parts = find_partitions(lopts.dst_image, scan_all_partitions, &elems);
        if (!parts) {
            perror("Unable to list partitions:");
            free(lopts.src_dirs);
            return EXIT_FAILURE;
        }
        if (lopts.partition_num - 1 > (int)elems) {
            fprintf(stderr, "Invalid partition selected. Max partition_num is: %lu\n", elems);
            free(parts);
            free(lopts.src_dirs);
            return EXIT_FAILURE;
        }
        const struct partition *curr_part = &parts[lopts.partition_num - 1];
        cfg.block_count                   = (curr_part->end - curr_part->start) / lopts.block_size;
        ioctx                             = lfs_ioaccess_open(&cfg, lopts.dst_image, curr_part);
        if (!ioctx) {
            fprintf(stderr, "Unable to open file: %s error %s\n", lopts.dst_image, strerror(errno));
            free(parts);
            free(lopts.src_dirs);
            return EXIT_FAILURE;
        }
        free(parts);
        if (write_partition_bootunit(lopts.dst_image, lopts.partition_num, lopts.block_size)) {
            perror("Unable to write bootunit");
            free(lopts.src_dirs);
            lfs_ioaccess_close(ioctx);
            return EXIT_FAILURE;
        }
    }
    else if (lopts.mode == littlefs_opts_file) {
        int fds = open(lopts.dst_image, O_CREAT | O_WRONLY, 0644);
        if (fds < 0) {
            perror("Unable to create file");
            free(lopts.src_dirs);
            return EXIT_FAILURE;
        }
        err = ftruncate(fds, lopts.filesystem_size);
        if (err) {
            perror("Unable to truncate file");
            close(fds);
            free(lopts.src_dirs);
            return EXIT_FAILURE;
        }
        close(fds);
        fds             = -1;
        cfg.block_count = lopts.filesystem_size / lopts.block_size;
        ioctx           = lfs_ioaccess_open(&cfg, lopts.dst_image, NULL);
        if (!ioctx) {
            perror("Unable to open file:");
            free(lopts.src_dirs);
            return EXIT_FAILURE;
        }
    }
    else {
        fprintf(stderr, "Unknown option\n");
        free(lopts.src_dirs);
        lfs_ioaccess_close(ioctx);
        return EXIT_FAILURE;
    }
    if (lopts.verbose) {
        print_config_options(&lopts);
    }

    if (!lopts.overwrite_existing && lfs_ioaccess_is_lfs_filesystem(ioctx) > 0) {
        fprintf(stderr, "LFS filesystem already exists. If you want to overwrite add --overwrite flag\n");
        free(lopts.src_dirs);
        lfs_ioaccess_close(ioctx);
        return EXIT_FAILURE;
    }

    err = lfs_format(&lfs, &cfg);
    if (err < 0) {
        fprintf(stderr, "lfs format error: error=%d\n", err);
        lfs_ioaccess_close(ioctx);
        free(lopts.src_dirs);
        return EXIT_FAILURE;
    }

    err = lfs_mount(&lfs, &cfg);
    if (err < 0) {
        fprintf(stderr, "lfs mount error: error=%d\n", err);
        lfs_ioaccess_close(ioctx);
        free(lopts.src_dirs);
        return EXIT_FAILURE;
    }

    for (size_t ndir = 0; ndir < lopts.src_dirs_siz; ++ndir) {
        err = add_to_lfs(&lfs, lopts.src_dirs[ndir], &prog_summary, lopts.verbose);
        if (err) {
            print_error(err, "Unable to open file: %s", lopts.src_dirs[ndir]);
            lfs_ioaccess_close(ioctx);
            free(lopts.src_dirs);
            lfs_unmount(&lfs);
            return EXIT_FAILURE;
        }
    }
    const lfs_ssize_t used_blocks = lfs_fs_size(&lfs);
    err                           = lfs_unmount(&lfs);
    if (err < 0) {
        fprintf(stderr, "lfs umount error: error=%d\n", err);
        lfs_ioaccess_close(ioctx);
        free(lopts.src_dirs);
        return EXIT_FAILURE;
    }
    free(lopts.src_dirs);
    lfs_ioaccess_close(ioctx);
    printf("Littlefs summary:\n"
           "     Directories created: %lu, Files added: %lu, Transferred %lu kbytes.\n"
           "     Littlefs block size: %i blocks: %i/%i.\n",
           prog_summary.directories_added,
           prog_summary.files_added,
           prog_summary.bytes_transferred / 1024UL,
           cfg.block_size,
           used_blocks,
           cfg.block_count);
    return EXIT_SUCCESS;
}
